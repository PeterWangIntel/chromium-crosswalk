// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_PUBLIC_CHILD_REQUEST_PEER_H_
#define CONTENT_PUBLIC_CHILD_REQUEST_PEER_H_

#include <string>

#include "base/basictypes.h"
#include "content/common/content_export.h"

class GURL;

namespace base {
class TimeTicks;
}

namespace net {
struct RedirectInfo;
}

namespace content {

struct ResourceResponseInfo;

// This is implemented by our custom resource loader within content. The Peer
// and it's bridge should have identical lifetimes as they represent each end of
// a communication channel.
//
// These callbacks mirror net::URLRequest::Delegate and the order and
// conditions in which they will be called are identical. See url_request.h
// for more information.
class CONTENT_EXPORT RequestPeer {
 public:
  // Called as upload progress is made.
  // note: only for requests with upload progress enabled.
  virtual void OnUploadProgress(uint64 position, uint64 size) = 0;

  // Called when a redirect occurs.  The implementation may return false to
  // suppress the redirect.  The ResourceResponseInfo provides information about
  // the redirect response and the RedirectInfo includes information about the
  // request to be made if the method returns true.
  virtual bool OnReceivedRedirect(const net::RedirectInfo& redirect_info,
                                  const ResourceResponseInfo& info) = 0;

  // Called when response headers are available (after all redirects have
  // been followed).
  virtual void OnReceivedResponse(const ResourceResponseInfo& info) = 0;

  // Called when a chunk of response data is downloaded.  This method may be
  // called multiple times or not at all if an error occurs.  This method is
  // only called if RequestInfo::download_to_file was set to true, and in
  // that case, OnReceivedData will not be called.
  // The encoded_data_length is the length of the encoded data transferred
  // over the network, which could be different from data length (e.g. for
  // gzipped content).
  virtual void OnDownloadedData(int len, int encoded_data_length) = 0;

  // Called when a chunk of response data is available. This method may
  // be called multiple times or not at all if an error occurs.
  // The encoded_data_length is the length of the encoded data transferred
  // over the network, which could be different from data length (e.g. for
  // gzipped content).
  virtual void OnReceivedData(const char* data,
                              int data_length,
                              int encoded_data_length) = 0;

  // Called when metadata generated by the renderer is retrieved from the
  // cache. This method may be called zero or one times.
  virtual void OnReceivedCachedMetadata(const char* data, int len) {}

  // Called when the response is complete.  This method signals completion of
  // the resource load.
  virtual void OnCompletedRequest(int error_code,
                                  bool was_ignored_by_handler,
                                  bool stale_copy_in_cache,
                                  const std::string& security_info,
                                  const base::TimeTicks& completion_time,
                                  int64 total_transfer_size) = 0;

  // This is a combined notification of
  //  - OnReceivedResponse,
  //  - OnReceivedData and
  //  - OnCompletedRequest.
  // Unlike OnReceivedData, |data| can be null.
  // This method is introduced to avoid repetitive method calls which might
  // lead to use-after-free issues. See https://crbug.com/485413,
  // https://crbug.com/507170.
  // TODO(yhirano): Fix the RequestPeer lifecycle problem and remove this
  // function.
  virtual void OnReceivedCompletedResponse(
      const ResourceResponseInfo& info,
      const char* data,
      int data_length,
      int encoded_data_length,
      int error_code,
      bool was_ignored_by_handler,
      bool stale_copy_in_cache,
      const std::string& security_info,
      const base::TimeTicks& completion_time,
      int64 total_transfer_size) = 0;

 protected:
  virtual ~RequestPeer() {}
};

}  // namespace content

#endif  // CONTENT_PUBLIC_CHILD_REQUEST_PEER_H_
