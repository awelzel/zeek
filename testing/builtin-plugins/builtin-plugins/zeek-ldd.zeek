# @TEST-DOC: Run ldd on the zeek excecutable and check for Kafka/HTTP2 dependencies being linked dynamically.
# @TEST-EXEC: ldd $(which zeek) > ldd.out
# @TEST-EXEC: grep librdkafka ldd.out
# @TEST-EXEC: grep libbrotli ldd.out
# @TEST-EXEC: grep libnghttp2 ldd.out
