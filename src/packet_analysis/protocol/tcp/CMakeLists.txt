
include(ZeekPlugin)

include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

zeek_plugin_begin(PacketAnalyzer TCP_PKT)
zeek_plugin_cc(TCP.cc TCPSessionAdapter.cc Plugin.cc Stats.cc)
zeek_plugin_end()
