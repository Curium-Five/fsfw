#include "fsfw/modes/ModeMessage.h"

Mode_t ModeMessage::getMode(const CommandMessage* message) { return message->getParameter(); }

Submode_t ModeMessage::getSubmode(const CommandMessage* message) {
  return message->getParameter2();
}

void ModeMessage::setModeMessage(CommandMessage* message, Command_t command, Mode_t mode,
                                 Submode_t submode) {
  message->setCommand(command);
  message->setParameter(mode);
  message->setParameter2(submode);
}

ReturnValue_t ModeMessage::getCantReachModeReason(const CommandMessage* message) {
  return message->getParameter();
}

void ModeMessage::clear(CommandMessage* message) { message->setCommand(CommandMessage::CMD_NONE); }

void ModeMessage::setCantReachMode(CommandMessage* message, ReturnValue_t reason) {
  message->setCommand(REPLY_CANT_REACH_MODE);
  message->setParameter(reason);
  message->setParameter2(0);
}

void ModeMessage::setModeAnnounceMessage(CommandMessage& message, bool recursive) {
  Command_t cmd;
  if (recursive) {
    cmd = CMD_MODE_ANNOUNCE_RECURSIVELY;
  } else {
    cmd = CMD_MODE_ANNOUNCE;
  }
  message.setCommand(cmd);
}

void ModeMessage::setCmdModeMessage(CommandMessage& message, Mode_t mode, Submode_t submode) {
  setModeMessage(&message, CMD_MODE_COMMAND, mode, submode);
}

void ModeMessage::setModeReadMessage(CommandMessage& message) { message.setCommand(CMD_MODE_READ); }
