// Copyright (c) 2020 Slack Technologies, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef SHELL_BROWSER_API_MESSAGE_PORT_H_
#define SHELL_BROWSER_API_MESSAGE_PORT_H_

#include <memory>
#include <vector>

#include "gin/wrappable.h"
#include "mojo/public/cpp/bindings/connector.h"
#include "mojo/public/cpp/bindings/message.h"
#include "third_party/blink/public/common/messaging/message_port_channel.h"

namespace gin {
class Arguments;
template <typename T>
class Handle;
}  // namespace gin

namespace electron {

// A non-blink version of blink::MessagePort.
class MessagePort : public gin::Wrappable<MessagePort>, mojo::MessageReceiver {
 public:
  ~MessagePort() override;
  static gin::Handle<MessagePort> Create(v8::Isolate* isolate);

  void PostMessage(gin::Arguments* args);
  void Start();
  void Close();

  void Entangle(mojo::ScopedMessagePipeHandle handle);
  void Entangle(blink::MessagePortChannel channel);

  blink::MessagePortChannel Disentangle();

  bool IsEntangled() const { return !closed_ && !IsNeutered(); }
  bool IsNeutered() const { return !connector_ || !connector_->is_valid(); }

  static std::vector<gin::Handle<MessagePort>> EntanglePorts(
      v8::Isolate* isolate,
      std::vector<blink::MessagePortChannel> channels);

  static std::vector<blink::MessagePortChannel> DisentanglePorts(
      v8::Isolate* isolate,
      const std::vector<gin::Handle<MessagePort>>& ports);

  // gin::Wrappable
  gin::ObjectTemplateBuilder GetObjectTemplateBuilder(
      v8::Isolate* isolate) override;
  static gin::WrapperInfo kWrapperInfo;

 private:
  MessagePort();

  // mojo::MessageReceiver
  bool Accept(mojo::Message* mojo_message) override;

  std::unique_ptr<mojo::Connector> connector_;
  bool started_ = false;
  bool closed_ = false;

  base::WeakPtrFactory<MessagePort> weak_factory_{this};
};

}  // namespace electron

#endif  // SHELL_BROWSER_API_MESSAGE_PORT_H_
