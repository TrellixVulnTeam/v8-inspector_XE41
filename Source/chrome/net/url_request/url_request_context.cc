// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/url_request/url_request_context.h"

#include "base/compiler_specific.h"
#include "base/debug/alias.h"
#include "base/debug/stack_trace.h"
#include "base/strings/string_util.h"
#include "net/cookies/cookie_store.h"
#include "net/dns/host_resolver.h"
#include "net/http/http_transaction_factory.h"
#include "net/url_request/http_user_agent_settings.h"
#include "net/url_request/url_request.h"

namespace net {

URLRequestContext::URLRequestContext()
    : net_log_(nullptr),
      host_resolver_(nullptr),
      cert_verifier_(nullptr),
      channel_id_service_(nullptr),
      fraudulent_certificate_reporter_(nullptr),
      http_auth_handler_factory_(nullptr),
      proxy_service_(nullptr),
      network_delegate_(nullptr),
      http_user_agent_settings_(nullptr),
      transport_security_state_(nullptr),
      cert_transparency_verifier_(nullptr),
      http_transaction_factory_(nullptr),
      job_factory_(nullptr),
      throttler_manager_(nullptr),
      // For investigation of http://crbug.com/454198; restore when resolved.
      // sdch_manager_(nullptr),
      have_sdch_manager_(false),
      network_quality_estimator_(nullptr),
      url_requests_(new std::set<const URLRequest*>) {
}

URLRequestContext::~URLRequestContext() {
  AssertNoURLRequests();
}

void URLRequestContext::CopyFrom(const URLRequestContext* other) {
  // Copy URLRequestContext parameters.
  set_net_log(other->net_log_);
  set_host_resolver(other->host_resolver_);
  set_cert_verifier(other->cert_verifier_);
  set_channel_id_service(other->channel_id_service_);
  set_fraudulent_certificate_reporter(other->fraudulent_certificate_reporter_);
  set_http_auth_handler_factory(other->http_auth_handler_factory_);
  set_proxy_service(other->proxy_service_);
  set_ssl_config_service(other->ssl_config_service_.get());
  set_network_delegate(other->network_delegate_);
  set_http_server_properties(other->http_server_properties_);
  set_cookie_store(other->cookie_store_.get());
  set_transport_security_state(other->transport_security_state_);
  set_cert_transparency_verifier(other->cert_transparency_verifier_);
  set_http_transaction_factory(other->http_transaction_factory_);
  set_job_factory(other->job_factory_);
  set_throttler_manager(other->throttler_manager_);
  // For investigation of http://crbug.com/454198; remove when resolved.
  CHECK(!other->have_sdch_manager_ || other->sdch_manager_.get());
  set_sdch_manager(other->sdch_manager_.get());
  set_http_user_agent_settings(other->http_user_agent_settings_);
  set_network_quality_estimator(other->network_quality_estimator_);
}

const HttpNetworkSession::Params* URLRequestContext::GetNetworkSessionParams(
    ) const {
  HttpTransactionFactory* transaction_factory = http_transaction_factory();
  if (!transaction_factory)
    return nullptr;
  HttpNetworkSession* network_session = transaction_factory->GetSession();
  if (!network_session)
    return nullptr;
  return &network_session->params();
}

scoped_ptr<URLRequest> URLRequestContext::CreateRequest(
    const GURL& url,
    RequestPriority priority,
    URLRequest::Delegate* delegate) const {
  return make_scoped_ptr(
      new URLRequest(url, priority, delegate, this, network_delegate_));
}

void URLRequestContext::set_cookie_store(CookieStore* cookie_store) {
  cookie_store_ = cookie_store;
}

void URLRequestContext::AssertNoURLRequests() const {
  int num_requests = url_requests_->size();
  if (num_requests != 0) {
    // We're leaking URLRequests :( Dump the URL of the first one and record how
    // many we leaked so we have an idea of how bad it is.
    char url_buf[128];
    const URLRequest* request = *url_requests_->begin();
    base::strlcpy(url_buf, request->url().spec().c_str(), arraysize(url_buf));
    bool has_delegate = request->has_delegate();
    int load_flags = request->load_flags();
    base::debug::StackTrace stack_trace(NULL, 0);
    if (request->stack_trace())
      stack_trace = *request->stack_trace();
    base::debug::Alias(url_buf);
    base::debug::Alias(&num_requests);
    base::debug::Alias(&has_delegate);
    base::debug::Alias(&load_flags);
    base::debug::Alias(&stack_trace);
    CHECK(false) << "Leaked " << num_requests << " URLRequest(s). First URL: "
                 << request->url().spec().c_str() << ".";
  }
}

}  // namespace net
