#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/csma-module.h"
#include "ns3/bridge-module.h"


#include "reliable-udp-client-helper.h"
#include "reliable-udp-client.h"
#include "reliable-udp-server-helper.h"
#include "reliable-udp-server.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("master");

int 
main (int argc, char *argv[])
{

	NodeContainer nodes;
	nodes.Create(2);

	std::string errorModelType = "ns3::RateErrorModel";
	Config::SetDefault ("ns3::RateErrorModel::ErrorRate", DoubleValue (0.001));
	Config::SetDefault ("ns3::RateErrorModel::ErrorUnit", StringValue ("ERROR_UNIT_PACKET"));

	PointToPointHelper p2p;
	p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
	p2p.SetChannelAttribute("Delay", StringValue("2ms"));

	NetDeviceContainer devices;
	devices = p2p.Install(nodes);

	InternetStackHelper stack;
	stack.Install(nodes);

	Ipv4AddressHelper addr;
	addr.SetBase("10.1.1.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces = addr.Assign(devices);

	ReliableUdpClientHelper rclient(interfaces.GetAddress(1), 9);
	rclient.SetAttribute("MaxPackets", UintegerValue(100000));
	rclient.SetAttribute("Interval", TimeValue(Seconds(0.1)));
	rclient.SetAttribute("PacketSize", UintegerValue(1024));

	ApplicationContainer clientApps;
	clientApps.Add(rclient.Install(nodes.Get(0)));
	clientApps.Start(Seconds(1.0));
	clientApps.Stop(Seconds(10.0));

	ReliableUdpServerHelper rserver(9);
	ApplicationContainer serverApps(rserver.Install(nodes.Get(1)));
	serverApps.Start(Seconds(0));
	serverApps.Stop(Seconds(11.0));

	ObjectFactory factory;
	factory.SetTypeId (errorModelType);
	Ptr<ErrorModel> em = factory.Create<ErrorModel> ();
	devices.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));

	p2p.EnablePcapAll("master", false);

	Simulator::Stop (Seconds (20));
	Simulator::Run ();
	Simulator::Destroy ();

}
