/*
 * Copyright (c) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "bindings/core/v8/WorkerThreadDebugger.h"

#include "bindings/core/v8/V8ScriptRunner.h"
#include "core/inspector/ScriptDebugListener.h"
#include "core/inspector/WorkerDebuggerAgent.h"
#include <v8.h>

namespace blink {

static const char* workerContextDebugId = "[worker]";

WorkerThreadDebugger::WorkerThreadDebugger(v8::Isolate* isolate, PassOwnPtr<ClientMessageLoop> clientMessageLoop)
    : ScriptDebuggerBase(isolate, V8Debugger::create(isolate, this))
    , m_clientMessageLoop(clientMessageLoop)
    , m_listener(nullptr)
{
}

WorkerThreadDebugger::~WorkerThreadDebugger()
{
}

void WorkerThreadDebugger::setContextDebugData(v8::Local<v8::Context> context)
{
    V8Debugger::setContextDebugData(context, workerContextDebugId);
}

void WorkerThreadDebugger::addListener(ScriptDebugListener* listener)
{
    ASSERT(!m_listener);
    debugger()->enable();
    m_listener = listener;
    debugger()->reportCompiledScripts(workerContextDebugId, listener);
}

void WorkerThreadDebugger::removeListener(ScriptDebugListener* listener)
{
    ASSERT(m_listener == listener);
    debugger()->continueProgram();
    m_listener = 0;
    debugger()->disable();
}

ScriptDebugListener* WorkerThreadDebugger::getDebugListenerForContext(v8::Local<v8::Context>)
{
    // There is only one worker context in isolate.
    return m_listener;
}

void WorkerThreadDebugger::runMessageLoopOnPause(v8::Local<v8::Context>)
{
    // Wait for continue or step command.
    m_clientMessageLoop->run();
    if (m_listener)
        m_listener->didContinue();
}

void WorkerThreadDebugger::quitMessageLoopOnPause()
{
    m_clientMessageLoop->quitNow();
}

} // namespace blink
