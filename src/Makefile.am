ACLOCAL_AMFLAGS=-I m4
lib_LTLIBRARIES = libcurl_system.la
libcurl_system_la_SOURCES = curl_system.cpp
libcurl_system_la_CXXFLAGS = -std=c++17 ${LIBECS_CFLAGS} ${JSONCPP_CFLAGS} ${LIBTHE_SEED_CFLAGS} ${LIBCURL_CFLAGS}
libcurl_system_la_LDFLAGS = -no-undefined
libcurl_system_la_LIBADD = ${LIBECS_LIBS} ${JSONCPP_LIBS} ${LIBTHE_SEED_LIBS} ${LIBCURL_LIBS} \
	../node_modules/@imperian-systems/http_request_component/src/libhttp_request_component.la \
	../node_modules/@imperian-systems/http_response_component/src/libhttp_response_component.la
