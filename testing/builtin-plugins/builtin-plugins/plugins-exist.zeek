# @TEST-DOC: Assumes and checks that bro-http2, zeek-kafka and CommunityID have been built into Zeek.
# @TEST-EXEC: zeek -N Corelight::CommunityID >>out
# @TEST-EXEC: zeek -N Seiso::Kafka >>out
# @TEST-EXEC: zeek -N mitrecnd::HTTP2 >>out
# @TEST-EXEC: zeek -N ICSNPP::BACnet >>out
# @TEST-EXEC: btest-diff out
