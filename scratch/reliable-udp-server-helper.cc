#include "reliable-udp-server-helper.h"
#include "ns3/uinteger.h"
#include "ns3/names.h"

namespace ns3 {

    ReliableUdpServerHelper::ReliableUdpServerHelper() {
        m_factory.SetTypeId(ReliableUdpServerHelper::GetTypeId());
    }

    ReliableUdpServerHelper::ReliableUdpServerHelper(uint16_t port) {
        m_factory.SetTypeId(ReliableUdpServerHelper::GetTypeId());
        SetAttribute("Port", UintegerValue(port));
    }

    void
    ReliableUdpServerHelper::SetAttribute(
            std::string name,
            const AttributeValue &value) {
        m_factory.Set(name, value);
    }

    ApplicationContainer
    ReliableUdpServerHelper::Install(Ptr <Node> node) const {
        return ApplicationContainer(InstallPriv(node));
    }

    ApplicationContainer
    ReliableUdpServerHelper::Install(NodeContainer c) const {
        ApplicationContainer apps;
        for (NodeContainer::Iterator i = c.Begin(); i != c.End(); ++i) {
            apps.Add(InstallPriv(*i));
        }

        return apps;
    }

    Ptr <Application>
    ReliableUdpServerHelper::InstallPriv(Ptr <Node> node) const {
        Ptr <Application> app = m_factory.Create<ReliableUdpServerHelper>();
        node->AddApplication(app);

        return app;
    }
} // namespace ns3
