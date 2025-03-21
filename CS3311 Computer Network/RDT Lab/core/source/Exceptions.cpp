#include "Exceptions.h"

using std::string_view, std::format;

WSAError::WSAError(string_view message) : mMessage(format("WSA error: {}", message)) {}
const char *WSAError::what() const noexcept { return mMessage.c_str(); }

SocketError::SocketError(string_view message) : mMessage(format("Socket error: {}", message)) {}
const char *SocketError::what() const noexcept { return mMessage.c_str(); }

ConnectionError::ConnectionError(string_view message) : mMessage(format("Connection error: {}", message)) {}
const char *ConnectionError::what() const noexcept { return mMessage.c_str(); }

ProtocolError::ProtocolError(string_view message) : mMessage(format("Protocol error: {}", message)) {}
const char *ProtocolError::what() const noexcept { return mMessage.c_str(); }

SystemError::SystemError(string_view message) : mMessage(format("System error: {}", message)) {}
const char *SystemError::what() const noexcept { return mMessage.c_str(); }

ChecksumError::ChecksumError(std::string_view message) : mMessage(std::format("Checksum error: {}", message)) {}
const char *ChecksumError::what() const noexcept { return mMessage.c_str(); }