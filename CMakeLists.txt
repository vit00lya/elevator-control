cmake_minimum_required(VERSION 3.18)
project(ElevatorControl)

set(CMAKE_CXX_STANDARD 20)
set(TARGET ElevatorControl)

list(APPEND SOURCES
	src/xserial.cpp
        src/xserial.hpp
	src/elevator_control.cpp
	src/elevator_control.h
	src/input_reader.cpp
	src/input_reader.h
	src/json_reader.h
	src/json_reader.cpp
	src/json.h
	src/json.cpp
	src/json_builder.cpp
	src/json_builder.h
	src/magic_enum.hpp
	src/main.cpp
	src/neon/config.h
	src/neon/memleak.h
	src/neon/mktable.c
	src/neon/ne_207.c
	src/neon/ne_207.h
	src/neon/ne_acl.h 
	src/neon/ne_acl3744.c
	src/neon/ne_acl3744.h
	src/neon/ne_alloc.c
	src/neon/ne_alloc.h
	src/neon/ne_auth.c
	src/neon/ne_auth.h
	src/neon/ne_basic.c
	src/neon/ne_basic.h
	src/neon/ne_compress.c
	src/neon/ne_compress.h
	src/neon/ne_dates.c
	src/neon/ne_dates.h
	src/neon/ne_defs.h
	src/neon/ne_gnutls.c
	src/neon/ne_i18n.c
	src/neon/ne_i18n.h
	src/neon/ne_internal.h
	src/neon/ne_locks.c
	src/neon/ne_locks.h
	src/neon/ne_md5.c
	src/neon/ne_md5.h
	src/neon/ne_ntlm.c
	src/neon/ne_ntlm.h
	src/neon/ne_openssl.c
	src/neon/ne_pkcs11.c
	src/neon/ne_pkcs11.h
	src/neon/ne_private.h
	src/neon/ne_privssl.h
	src/neon/ne_props.c
	src/neon/ne_props.h
	src/neon/ne_redirect.c
	src/neon/ne_redirect.h
	src/neon/ne_request.c
	src/neon/ne_request.h
	src/neon/ne_session.c
	src/neon/ne_session.h
	src/neon/ne_socket.c
	src/neon/ne_socket.h
	src/neon/ne_socks.c
	src/neon/ne_ssl.h
	src/neon/ne_sspi.c
	src/neon/ne_sspi.h
	src/neon/ne_string.c
	src/neon/ne_string.h
	src/neon/ne_stubssl.c
	src/neon/ne_uri.c
	src/neon/ne_uri.h
	src/neon/ne_utils.c
	src/neon/ne_utils.h
	src/neon/ne_xml.c
	src/neon/ne_xml.h
	src/neon/ne_xmlreq.c
	src/neon/ne_xmlreq.h)

add_executable(${TARGET} ${SOURCES})
