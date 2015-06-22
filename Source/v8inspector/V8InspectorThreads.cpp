// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "v8inspector/V8InspectorThreads.h"

namespace v8inspector {

base::MessageLoop* V8InspectorThreads::m_mainThreadLoop = nullptr;

base::MessageLoop* V8InspectorThreads::m_ioThreadLoop = nullptr;


} // namespace v8inspector