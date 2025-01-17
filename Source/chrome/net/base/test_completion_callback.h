// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_BASE_TEST_COMPLETION_CALLBACK_H_
#define NET_BASE_TEST_COMPLETION_CALLBACK_H_

#include "base/callback.h"
#include "base/compiler_specific.h"
#include "base/memory/scoped_ptr.h"
#include "net/base/completion_callback.h"
#include "net/base/net_errors.h"

//-----------------------------------------------------------------------------
// completion callback helper

// A helper class for completion callbacks, designed to make it easy to run
// tests involving asynchronous operations.  Just call WaitForResult to wait
// for the asynchronous operation to complete.  Uses a RunLoop to spin the
// current MessageLoop while waiting.  The callback must be invoked on the same
// thread WaitForResult is called on.
//
// NOTE: Since this runs a message loop to wait for the completion callback,
// there could be other side-effects resulting from WaitForResult.  For this
// reason, this class is probably not ideal for a general application.
//
namespace base {
class RunLoop;
}

namespace net {

class IOBuffer;

namespace internal {

class TestCompletionCallbackBaseInternal {
 public:
  bool have_result() const { return have_result_; }

 protected:
  TestCompletionCallbackBaseInternal();
  virtual ~TestCompletionCallbackBaseInternal();

  void DidSetResult();
  void WaitForResult();

 private:
  // RunLoop.  Only non-NULL during the call to WaitForResult, so the class is
  // reusable.
  scoped_ptr<base::RunLoop> run_loop_;
  bool have_result_;

  DISALLOW_COPY_AND_ASSIGN(TestCompletionCallbackBaseInternal);
};

template <typename R>
class TestCompletionCallbackTemplate
    : public TestCompletionCallbackBaseInternal {
 public:
  virtual ~TestCompletionCallbackTemplate() override {}

  R WaitForResult() {
    TestCompletionCallbackBaseInternal::WaitForResult();
    return result_;
  }

  R GetResult(R result) {
    if (ERR_IO_PENDING != result)
      return result;
    return WaitForResult();
  }

 protected:
  TestCompletionCallbackTemplate() : result_(R()) {}

  // Override this method to gain control as the callback is running.
  virtual void SetResult(R result) {
    result_ = result;
    DidSetResult();
  }

 private:
  R result_;

  DISALLOW_COPY_AND_ASSIGN(TestCompletionCallbackTemplate);
};

}  // namespace internal

class TestClosure : public internal::TestCompletionCallbackBaseInternal {
 public:
  using internal::TestCompletionCallbackBaseInternal::WaitForResult;

  TestClosure();
  ~TestClosure() override;

  const base::Closure& closure() const { return closure_; }

 private:
  const base::Closure closure_;

  DISALLOW_COPY_AND_ASSIGN(TestClosure);
};

// Base class overridden by custom implementations of TestCompletionCallback.
typedef internal::TestCompletionCallbackTemplate<int>
    TestCompletionCallbackBase;

typedef internal::TestCompletionCallbackTemplate<int64>
    TestInt64CompletionCallbackBase;

class TestCompletionCallback : public TestCompletionCallbackBase {
 public:
  TestCompletionCallback();
  ~TestCompletionCallback() override;

  const CompletionCallback& callback() const { return callback_; }

 private:
  const CompletionCallback callback_;

  DISALLOW_COPY_AND_ASSIGN(TestCompletionCallback);
};

class TestInt64CompletionCallback : public TestInt64CompletionCallbackBase {
 public:
  TestInt64CompletionCallback();
  ~TestInt64CompletionCallback() override;

  const Int64CompletionCallback& callback() const { return callback_; }

 private:
  const Int64CompletionCallback callback_;

  DISALLOW_COPY_AND_ASSIGN(TestInt64CompletionCallback);
};

// Makes sure that the buffer is not referenced when the callback runs.
class ReleaseBufferCompletionCallback: public TestCompletionCallback {
 public:
  explicit ReleaseBufferCompletionCallback(IOBuffer* buffer);
  ~ReleaseBufferCompletionCallback() override;

 private:
  void SetResult(int result) override;

  IOBuffer* buffer_;
  DISALLOW_COPY_AND_ASSIGN(ReleaseBufferCompletionCallback);
};

}  // namespace net

#endif  // NET_BASE_TEST_COMPLETION_CALLBACK_H_
