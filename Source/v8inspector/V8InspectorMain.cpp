// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "bindings/core/v8/ScriptState.h"
#include "bindings/core/v8/WorkerThreadDebugger.h"
#include "v8inspector/V8Inspector.h"
#include "v8inspector/RemoteDebuggingServer.h"
#include "wtf/OwnPtr.h"

#include <include/v8.h>
#include <include/libplatform/libplatform.h>

#include "base/message_loop/message_loop.h"
#include "base/at_exit.h"
#include "base/run_loop.h"
#include "base/threading/thread.h"
#include "base/bind.h"

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace blink;
using namespace v8inspector;

/**
 * This sample program shows how to implement a simple javascript shell
 * based on V8.  This includes initializing V8 with command line options,
 * creating global functions, compiling and executing strings.
 *
 * For a more sophisticated shell, consider using the debug shell D8.
 */


v8::Handle<v8::Context> CreateShellContext(v8::Isolate* isolate);
void RunShell(v8::Handle<v8::Context> context);
int RunMain(v8::Isolate* isolate, int argc, char* argv[]);
bool ExecuteString(v8::Isolate* isolate,
                   v8::Handle<v8::String> source,
                   v8::Handle<v8::Value> name,
                   bool print_result,
                   bool report_exceptions);
void Print(const v8::FunctionCallbackInfo<v8::Value>& args);
void Read(const v8::FunctionCallbackInfo<v8::Value>& args);
void Load(const v8::FunctionCallbackInfo<v8::Value>& args);
void Quit(const v8::FunctionCallbackInfo<v8::Value>& args);
void Version(const v8::FunctionCallbackInfo<v8::Value>& args);
v8::Handle<v8::String> ReadFile(v8::Isolate* isolate, const char* name);
void ReportException(v8::Isolate* isolate, v8::TryCatch* handler);


static bool run_shell;

namespace {

class DebuggerMessageLoopImpl : public WorkerThreadDebugger::ClientMessageLoop {
 public:
  DebuggerMessageLoopImpl() : message_loop_(base::MessageLoop::current()) {}
  virtual ~DebuggerMessageLoopImpl() { message_loop_ = NULL; }
  void run() override {
    base::MessageLoop::ScopedNestableTaskAllower allow(message_loop_);
    message_loop_->Run();
  }
  void quitNow() override {
    message_loop_->QuitNow();
  }
 private:
  base::MessageLoop* message_loop_;
};

class ShellArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
 public:
  void* Allocate(size_t length) override {
    void* data = AllocateUninitialized(length);
    return data == NULL ? data : memset(data, 0, length);
  }
  void* AllocateUninitialized(size_t length) override { return malloc(length); }
  void Free(void* data, size_t) override { free(data); }
};


static void runMainTask(v8::Isolate* isolate, int argc, char* argv[], int* result)
{
    *result = RunMain(isolate, argc, argv);
    if (run_shell) {
        v8::Local<v8::Context> context = isolate->GetCurrentContext();
        RunShell(context);
    }
}

void quitWhenIdle(const v8::FunctionCallbackInfo<v8::Value>& args) {
    run_shell = false;
    base::MessageLoop::current()->QuitWhenIdle();
}

}

int main(int argc, char* argv[]) {
  base::AtExitManager at_exit;
  base::MessageLoop message_loop;

  v8::V8::InitializeICU();
  v8::Platform* platform = v8::platform::CreateDefaultPlatform();
  v8::V8::InitializePlatform(platform);
  v8::V8::Initialize();
  v8::V8::SetFlagsFromCommandLine(&argc, argv, true);
  ShellArrayBufferAllocator array_buffer_allocator;
  v8::Isolate::CreateParams create_params;
  create_params.array_buffer_allocator = &array_buffer_allocator;
  v8::Isolate* isolate = v8::Isolate::New(create_params);

  run_shell = (argc == 1);

  fprintf(stderr, "main 10\n");
  int result;
  {
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);
    v8::Handle<v8::Context> context = CreateShellContext(isolate);
    // Set context deubg data to see its scripts in the debugger.
    WorkerThreadDebugger::setContextDebugData(context);
    if (context.IsEmpty()) {
      fprintf(stderr, "Error creating context\n");
      return 1;
    }
    v8::Context::Scope context_scope(context);

    // Must be in context when constructing V8Inspector.
    ScriptState::create(context);
    OwnPtr<V8Inspector> inspector = adoptPtr(new V8Inspector(isolate, adoptPtr(new DebuggerMessageLoopImpl())));
    fprintf(stderr, "V8 inspector is running\n");
    scoped_ptr<RemoteDebuggingServer> server(new RemoteDebuggingServer(inspector.get()));

    message_loop.task_runner()->PostTask(
        FROM_HERE,
        base::Bind(&runMainTask, base::Unretained(isolate), argc, base::Unretained(argv), base::Unretained(&result)));

    fprintf(stderr, "Start running main loop\n");
    base::RunLoop run_loop;
    run_loop.Run();
    fprintf(stderr, "Exited main loop\n");
  }
  isolate->Dispose();
  v8::V8::Dispose();
  v8::V8::ShutdownPlatform();
  delete platform;
  fprintf(stderr, "Deleted platform.\n");
  return result;
}


// Extracts a C string from a V8 Utf8Value.
const char* ToCString(const v8::String::Utf8Value& value) {
  return *value ? *value : "<string conversion failed>";
}


// Creates a new execution environment containing the built-in
// functions.
v8::Handle<v8::Context> CreateShellContext(v8::Isolate* isolate) {
  // Create a template for the global object.
  v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
  // Bind the global 'print' function to the C++ Print callback.
  global->Set(v8::String::NewFromUtf8(isolate, "print"),
              v8::FunctionTemplate::New(isolate, Print));
  // Bind the global 'read' function to the C++ Read callback.
  global->Set(v8::String::NewFromUtf8(isolate, "read"),
              v8::FunctionTemplate::New(isolate, Read));
  // Bind the global 'load' function to the C++ Load callback.
  global->Set(v8::String::NewFromUtf8(isolate, "load"),
              v8::FunctionTemplate::New(isolate, Load));
  // Bind the 'quit' function
  global->Set(v8::String::NewFromUtf8(isolate, "quit"),
              v8::FunctionTemplate::New(isolate, Quit));
  // Bind the 'version' function
  global->Set(v8::String::NewFromUtf8(isolate, "version"),
              v8::FunctionTemplate::New(isolate, Version));

  // Bind the 'quitWhenIdle' function
  global->Set(v8::String::NewFromUtf8(isolate, "quitWhenIdle"),
              v8::FunctionTemplate::New(isolate, quitWhenIdle));

  return v8::Context::New(isolate, NULL, global);
}


// The callback that is invoked by v8 whenever the JavaScript 'print'
// function is called.  Prints its arguments on stdout separated by
// spaces and ending with a newline.
void Print(const v8::FunctionCallbackInfo<v8::Value>& args) {
  bool first = true;
  for (int i = 0; i < args.Length(); i++) {
    v8::HandleScope handle_scope(args.GetIsolate());
    if (first) {
      first = false;
    } else {
      printf(" ");
    }
    v8::String::Utf8Value str(args[i]);
    const char* cstr = ToCString(str);
    printf("%s", cstr);
  }
  printf("\n");
  fflush(stdout);
}


// The callback that is invoked by v8 whenever the JavaScript 'read'
// function is called.  This function loads the content of the file named in
// the argument into a JavaScript string.
void Read(const v8::FunctionCallbackInfo<v8::Value>& args) {
  if (args.Length() != 1) {
    args.GetIsolate()->ThrowException(
        v8::String::NewFromUtf8(args.GetIsolate(), "Bad parameters"));
    return;
  }
  v8::String::Utf8Value file(args[0]);
  if (*file == NULL) {
    args.GetIsolate()->ThrowException(
        v8::String::NewFromUtf8(args.GetIsolate(), "Error loading file"));
    return;
  }
  v8::Handle<v8::String> source = ReadFile(args.GetIsolate(), *file);
  if (source.IsEmpty()) {
    args.GetIsolate()->ThrowException(
        v8::String::NewFromUtf8(args.GetIsolate(), "Error loading file"));
    return;
  }
  args.GetReturnValue().Set(source);
}


// The callback that is invoked by v8 whenever the JavaScript 'load'
// function is called.  Loads, compiles and executes its argument
// JavaScript file.
void Load(const v8::FunctionCallbackInfo<v8::Value>& args) {
  for (int i = 0; i < args.Length(); i++) {
    v8::HandleScope handle_scope(args.GetIsolate());
    v8::String::Utf8Value file(args[i]);
    if (*file == NULL) {
      args.GetIsolate()->ThrowException(
          v8::String::NewFromUtf8(args.GetIsolate(), "Error loading file"));
      return;
    }
    v8::Handle<v8::String> source = ReadFile(args.GetIsolate(), *file);
    if (source.IsEmpty()) {
      args.GetIsolate()->ThrowException(
           v8::String::NewFromUtf8(args.GetIsolate(), "Error loading file"));
      return;
    }
    if (!ExecuteString(args.GetIsolate(),
                       source,
                       v8::String::NewFromUtf8(args.GetIsolate(), *file),
                       false,
                       false)) {
      args.GetIsolate()->ThrowException(
          v8::String::NewFromUtf8(args.GetIsolate(), "Error executing file"));
      return;
    }
  }
}


// The callback that is invoked by v8 whenever the JavaScript 'quit'
// function is called.  Quits.
void Quit(const v8::FunctionCallbackInfo<v8::Value>& args) {
  // If not arguments are given args[0] will yield undefined which
  // converts to the integer value 0.
  int exit_code = args[0]->Int32Value();
  fflush(stdout);
  fflush(stderr);
  exit(exit_code);
}


void Version(const v8::FunctionCallbackInfo<v8::Value>& args) {
  args.GetReturnValue().Set(
      v8::String::NewFromUtf8(args.GetIsolate(), v8::V8::GetVersion()));
}


// Reads a file into a v8 string.
v8::Handle<v8::String> ReadFile(v8::Isolate* isolate, const char* name) {
  FILE* file = fopen(name, "rb");
  if (file == NULL) return v8::Handle<v8::String>();

  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  rewind(file);

  char* chars = new char[size + 1];
  chars[size] = '\0';
  for (size_t i = 0; i < size;) {
    i += fread(&chars[i], 1, size - i, file);
    if (ferror(file)) {
      fclose(file);
      return v8::Handle<v8::String>();
    }
  }
  fclose(file);
  v8::Handle<v8::String> result = v8::String::NewFromUtf8(
      isolate, chars, v8::String::kNormalString, static_cast<int>(size));
  delete[] chars;
  return result;
}


// Process remaining command line arguments and execute files
int RunMain(v8::Isolate* isolate, int argc, char* argv[]) {
  for (int i = 1; i < argc; i++) {
    const char* str = argv[i];
    if (strcmp(str, "--shell") == 0) {
      run_shell = true;
    } else if (strcmp(str, "-f") == 0) {
      // Ignore any -f flags for compatibility with the other stand-
      // alone JavaScript engines.
      continue;
    } else if (strncmp(str, "--", 2) == 0) {
      fprintf(stderr,
              "Warning: unknown flag %s.\nTry --help for options\n", str);
    } else if (strcmp(str, "-e") == 0 && i + 1 < argc) {
      // Execute argument given to -e option directly.
      v8::Handle<v8::String> file_name =
          v8::String::NewFromUtf8(isolate, "unnamed");
      v8::Handle<v8::String> source =
          v8::String::NewFromUtf8(isolate, argv[++i]);
      if (!ExecuteString(isolate, source, file_name, false, true)) return 1;
    } else {
      // Use all other arguments as names of files to load and run.
      v8::Handle<v8::String> file_name = v8::String::NewFromUtf8(isolate, str);
      v8::Handle<v8::String> source = ReadFile(isolate, str);
      if (source.IsEmpty()) {
        fprintf(stderr, "Error reading '%s'\n", str);
        continue;
      }
      if (!ExecuteString(isolate, source, file_name, false, true)) return 1;
    }
  }
  return 0;
}


// The read-eval-execute loop of the shell.
void RunShell(v8::Handle<v8::Context> context) {
  fprintf(stderr, "V8 version %s [sample shell]\n", v8::V8::GetVersion());
  static const int kBufferSize = 256;
  // Enter the execution environment before evaluating any code.
  v8::Context::Scope context_scope(context);
  v8::Local<v8::String> name(
      v8::String::NewFromUtf8(context->GetIsolate(), "(shell)"));
  while (run_shell) {
    char buffer[kBufferSize];
    fprintf(stderr, "> ");
    char* str = fgets(buffer, kBufferSize, stdin);
    if (str == NULL) break;
    v8::HandleScope handle_scope(context->GetIsolate());
    ExecuteString(context->GetIsolate(),
                  v8::String::NewFromUtf8(context->GetIsolate(), str),
                  name,
                  true,
                  true);
  }
  fprintf(stderr, "\n");
}


// Executes a string within the current v8 context.
bool ExecuteString(v8::Isolate* isolate,
                   v8::Handle<v8::String> source,
                   v8::Handle<v8::Value> name,
                   bool print_result,
                   bool report_exceptions) {
  v8::HandleScope handle_scope(isolate);
  v8::TryCatch try_catch;
  v8::ScriptOrigin origin(name);
  v8::Handle<v8::Script> script = v8::Script::Compile(source, &origin);
  if (script.IsEmpty()) {
    // Print errors that happened during compilation.
    if (report_exceptions)
      ReportException(isolate, &try_catch);
    return false;
  } else {
    v8::Handle<v8::Value> result = script->Run();
    if (result.IsEmpty()) {
      assert(try_catch.HasCaught());
      // Print errors that happened during execution.
      if (report_exceptions)
        ReportException(isolate, &try_catch);
      return false;
    } else {
      assert(!try_catch.HasCaught());
      if (print_result && !result->IsUndefined()) {
        // If all went well and the result wasn't undefined then print
        // the returned value.
        v8::String::Utf8Value str(result);
        const char* cstr = ToCString(str);
        printf("%s\n", cstr);
      }
      return true;
    }
  }
}


void ReportException(v8::Isolate* isolate, v8::TryCatch* try_catch) {
  v8::HandleScope handle_scope(isolate);
  v8::String::Utf8Value exception(try_catch->Exception());
  const char* exception_string = ToCString(exception);
  v8::Handle<v8::Message> message = try_catch->Message();
  if (message.IsEmpty()) {
    // V8 didn't provide any extra information about this error; just
    // print the exception.
    fprintf(stderr, "%s\n", exception_string);
  } else {
    // Print (filename):(line number): (message).
    v8::String::Utf8Value filename(message->GetScriptOrigin().ResourceName());
    const char* filename_string = ToCString(filename);
    int linenum = message->GetLineNumber();
    fprintf(stderr, "%s:%i: %s\n", filename_string, linenum, exception_string);
    // Print line of source code.
    v8::String::Utf8Value sourceline(message->GetSourceLine());
    const char* sourceline_string = ToCString(sourceline);
    fprintf(stderr, "%s\n", sourceline_string);
    // Print wavy underline (GetUnderline is deprecated).
    int start = message->GetStartColumn();
    for (int i = 0; i < start; i++) {
      fprintf(stderr, " ");
    }
    int end = message->GetEndColumn();
    for (int i = start; i < end; i++) {
      fprintf(stderr, "^");
    }
    fprintf(stderr, "\n");
    v8::String::Utf8Value stack_trace(try_catch->StackTrace());
    if (stack_trace.length() > 0) {
      const char* stack_trace_string = ToCString(stack_trace);
      fprintf(stderr, "%s\n", stack_trace_string);
    }
  }
}
