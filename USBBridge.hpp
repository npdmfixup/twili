#pragma once

#include<libtransistor/cpp/waiter.hpp>
#include<libtransistor/cpp/ipc/usb_ds.hpp>

namespace twili {

class Twili;

namespace usb {

class USBBridge {
	enum class State {
		INVALID,
		WAITING_ON_HEADER,
		WAITING_ON_PAYLOAD,
	};

	enum class CommandID : uint64_t {
		RUN = 10,
	};
	
	struct CommandHeader {
		uint64_t command_id;
		uint64_t payload_size;
	};
	
 public:
	USBBridge(Twili *twili);
	~USBBridge();
 private:
	Twili *twili;
	std::shared_ptr<Transistor::IPC::USB::DS::Interface> interface;
	std::shared_ptr<Transistor::IPC::USB::DS::Endpoint> endpoint_in;
	std::shared_ptr<Transistor::IPC::USB::DS::Endpoint> endpoint_out;

	void *incoming_buffer = nullptr; // remember, these are from the perspective of the host!
	void *outgoing_buffer = nullptr;

	Transistor::IPC::USB::DS::DS ds;
	Transistor::KEvent usb_state_change_event;
	void USBTransactionCompleted();
	bool USBStateChangeCallback();
	void BeginReadHeader();
	void BeginReadPayload();
	void ContinueReadingPayload();
	bool ValidateCommandHeader();
	bool ProcessCommand();
	
	State state = State::INVALID;
	CommandHeader current_header;
	uint8_t *current_payload;
	size_t payload_read;
	uint32_t recv_urb_id;

	std::shared_ptr<Transistor::WaitHandle> endpoint_out_completion_wait;
};

}
}