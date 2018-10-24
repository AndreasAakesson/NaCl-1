// This file is a part of the IncludeOS unikernel - www.includeos.org
//
// Copyright 2017-2018 IncludeOS AS, Oslo, Norway
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Autogenerated by NaCl

#include <iostream>
#include <net/interfaces>
#include <net/ip4/cidr.hpp>
#include <syslogd>

using namespace net;

namespace nacl {
  class Filter {
  public:
    virtual Filter_verdict<IP4> operator()(IP4::IP_packet_ptr pckt, Inet& stack, Conntrack::Entry_ptr ct_entry) = 0;
    virtual ~Filter() {}
  };
}

std::shared_ptr<Conntrack> nacl_ct_obj;

namespace custom_made_classes_from_nacl {

class My_Filter : public nacl::Filter {
public:
	Filter_verdict<IP4> operator()(IP4::IP_packet_ptr pckt, Inet& stack, Conntrack::Entry_ptr ct_entry) {
		if (not ct_entry) {
return {nullptr, Filter_verdict_type::DROP};
}
return {std::move(pckt), Filter_verdict_type::ACCEPT};

	}
};

} //< namespace custom_made_classes_from_nacl

void register_plugin_nacl() {
	INFO("NaCl", "Registering NaCl plugin");

	auto& eth0 = Interfaces::get(0);
	eth0.network_config(IP4::addr{10,0,0,45}, IP4::addr{255,255,255,0}, IP4::addr{10,0,0,1});

	custom_made_classes_from_nacl::My_Filter my_filter;

	eth0.ip_obj().prerouting_chain().chain.push_back(my_filter);

	// Ct

	nacl_ct_obj = std::make_shared<Conntrack>();
	nacl_ct_obj->tcp_in = tcp::tcp4_conntrack;
	nacl_ct_obj->maximum_entries = 20000;
	nacl_ct_obj->reserve(10000);
	nacl_ct_obj->timeout.established.tcp = Conntrack::Timeout_duration{ 100 };
	nacl_ct_obj->timeout.established.udp = Conntrack::Timeout_duration{ 200 };
	nacl_ct_obj->timeout.established.icmp = Conntrack::Timeout_duration{ 300 };
	nacl_ct_obj->timeout.confirmed.tcp = Conntrack::Timeout_duration{ 700 };
	nacl_ct_obj->timeout.confirmed.udp = Conntrack::Timeout_duration{ 800 };
	nacl_ct_obj->timeout.confirmed.icmp = Conntrack::Timeout_duration{ 900 };
	nacl_ct_obj->timeout.unconfirmed.tcp = Conntrack::Timeout_duration{ 400 };
	nacl_ct_obj->timeout.unconfirmed.udp = Conntrack::Timeout_duration{ 500 };
	nacl_ct_obj->timeout.unconfirmed.icmp = Conntrack::Timeout_duration{ 600 };

	INFO("NaCl", "Enabling Conntrack on eth0");
	eth0.enable_conntrack(nacl_ct_obj);
}
