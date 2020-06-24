// Copyright 2020 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef CORE_V2_INTERNAL_PCP_MANAGER_H_
#define CORE_V2_INTERNAL_PCP_MANAGER_H_

#include <string>

#include "core_v2/internal/client_proxy.h"
#include "core_v2/internal/endpoint_channel_manager.h"
#include "core_v2/internal/endpoint_manager.h"
#include "core_v2/internal/mediums/mediums.h"
#include "core_v2/internal/pcp_handler.h"
#include "core_v2/listeners.h"
#include "core_v2/options.h"
#include "core_v2/status.h"
#include "core_v2/strategy.h"
#include "absl/container/flat_hash_map.h"

namespace location {
namespace nearby {
namespace connections {

// Manages all known PcpHandler implementations, delegating operations to the
// appropriate one as per the parameters passed in.
//
// This will only ever be used by the OfflineServiceController, which has all
// of its entrypoints invoked serially, so there's no synchronization needed.
// Public method semantics matches definition in the
// cpp/core_v2/internal/service_controller.h
class PcpManager {
 public:
  PcpManager(Mediums& mediums, EndpointChannelManager& channel_manager,
             EndpointManager& endpoint_manager);
  ~PcpManager() = default;

  Status StartAdvertising(ClientProxy* client_proxy, const string& service_id,
                          const ConnectionOptions& options,
                          const ConnectionRequestInfo& info);
  void StopAdvertising(ClientProxy* client_proxy);

  Status StartDiscovery(ClientProxy* client_proxy, const string& service_id,
                        const ConnectionOptions& options,
                        DiscoveryListener listener);
  void StopDiscovery(ClientProxy* client_proxy);

  Status RequestConnection(ClientProxy* client_proxy, const string& endpoint_id,
                           const ConnectionRequestInfo& info);
  Status AcceptConnection(ClientProxy* client_proxy, const string& endpoint_id,
                          const PayloadListener& payload_listener);
  Status RejectConnection(ClientProxy* client_proxy, const string& endpoint_id);

  proto::connections::Medium GetBandwidthUpgradeMedium();

 private:
  bool SetCurrentPcpHandler(Strategy strategy);
  PcpHandler* GetPcpHandler(Pcp pcp) const;

  absl::flat_hash_map<Pcp, std::unique_ptr<PcpHandler>> handlers_;
  PcpHandler* current_;
};

}  // namespace connections
}  // namespace nearby
}  // namespace location

#endif  // CORE_V2_INTERNAL_PCP_MANAGER_H_
