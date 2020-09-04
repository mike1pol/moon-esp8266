import React, { useState } from "react";
import {
  Body,
  Button,
  Container,
  Content,
  Form,
  Header,
  Icon,
  Input,
  Item,
  Left,
  Right,
  Text,
  Title,
} from "native-base";
import { Modal } from "react-native";
import { StatusBar } from "expo-status-bar";
import { SettingsProps } from "./types";

export default function Settings({
  host,
  port,
  state,
  onClose,
}: SettingsProps) {
  const [h, setHost] = useState(host);
  const [p, setPort] = useState(port.toString());
  return (
    <Modal animationType="slide" visible={state}>
      <Container>
        <StatusBar style="auto" />
        <Header>
          <Left />
          <Body>
            <Title>Settings</Title>
          </Body>
          <Right>
            <Button transparent onPress={() => onClose()}>
              <Icon type="EvilIcons" name="close" />
            </Button>
          </Right>
        </Header>
        <Content padder>
          <Form>
            <Item last>
              <Input
                autoCompleteType={"off"}
                autoCorrect={false}
                keyboardType={"numbers-and-punctuation"}
                placeholder={"IP"}
                value={h}
                onChangeText={(v) => setHost(v)}
              />
            </Item>
            <Item last>
              <Input
                autoCompleteType={"off"}
                autoCorrect={false}
                keyboardType={"number-pad"}
                placeholder={"Port"}
                value={p}
                onChangeText={(v) => setPort(v)}
              />
            </Item>
            <Button
              style={{ marginTop: 20 }}
              onPress={() => onClose({ h, p })}
              full
              primary
            >
              <Text>Save</Text>
            </Button>
          </Form>
        </Content>
      </Container>
    </Modal>
  );
}
