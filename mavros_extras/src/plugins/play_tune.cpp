#include <mavros/mavros_plugin.h>
#include <mavros_msgs/PlayTuneV2.h>
#include <cstring>

namespace mavros
{
namespace extra_plugins
{

class PlayTunePlugin : public plugin::PluginBase
{
  public:
    PlayTunePlugin() : PluginBase(), nh("~") {}

    void initialize(UAS& uas_) override
    {
        PluginBase::initialize(uas_);
        sub = nh.subscribe("play_tune", 1, &PlayTunePlugin::callback, this);
    }

    Subscriptions get_subscriptions() override { return { /* No subscriptions */ }; }

  private:
    ros::NodeHandle nh;
    ros::Subscriber sub;

    void callback(const mavros_msgs::PlayTuneV2::ConstPtr& tune)
    {
        auto msg = mavlink::common::msg::PLAY_TUNE_V2{};
        msg.target_system = m_uas->get_tgt_system();
        msg.target_component = m_uas->get_tgt_component();

        switch (tune->format)
        {
            case mavros_msgs::PlayTuneV2::TUNE_FORMAT_QBASIC1_1:
                msg.format = static_cast<uint32_t>(mavlink::common::TUNE_FORMAT::QBASIC1_1);
                break;

            case mavros_msgs::PlayTuneV2::TUNE_FORMAT_MML_MODERN:
                msg.format = static_cast<uint32_t>(mavlink::common::TUNE_FORMAT::MML_MODERN);
                break;

            default:
                ROS_ERROR_NAMED("play_tune", "Invalid tune format: '%i'", tune->format);
                return;
        }

        std::strncpy(msg.tune.data(), tune->tune.c_str(), sizeof(msg.tune));
        msg.tune.back() = '\0';

        UAS_FCU(m_uas)->send_message_ignore_drop(msg);
    }
};

}  // namespace extra_plugins
}  // namespace mavros

#include <pluginlib/class_list_macros.h>
PLUGINLIB_EXPORT_CLASS(mavros::extra_plugins::PlayTunePlugin, mavros::plugin::PluginBase)