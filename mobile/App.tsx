import React from "react";
import { AppLoading } from "expo";
import { Dimensions } from "react-native";
import { StatusBar } from "expo-status-bar";
import Slider from "@react-native-community/slider";
import AsyncStorage from "@react-native-community/async-storage";
import {
  TriangleColorPicker,
  fromHsv,
  toHsv,
  HsvColor,
} from "react-native-color-picker";
import {
  Body,
  Button,
  Container,
  Header,
  Icon,
  Left,
  Right,
  Root,
  Title,
  Toast,
  View,
} from "native-base";
import * as Font from "expo-font";
import { EvilIcons, Ionicons } from "@expo/vector-icons";
import Settings from "./Settings";

const connectionHostKey = "@connectionHost";
const connectionPortKey = "@connectionPort";
const w = Dimensions.get("window").width - 50;

import { AppProps, AppState } from "./types";
import { hexToRgb, rgbToHex } from "./utils";

export default class App extends React.Component<AppProps, AppState> {
  ws: WebSocket | null = null;
  constructor(props: AppProps) {
    super(props);
    this.state = {
      host: "",
      port: "",
      color: { h: 0, s: 0, v: 1 },
      power: 0,
      bri: 100,
      state: "disconnected",
      settingsModal: false,
      isReady: false,
    };
    this.changeState = this.changeState.bind(this);
    this.changePower = this.changePower.bind(this);
    this.onClose = this.onClose.bind(this);
    this.onMessage = this.onMessage.bind(this);
    this.onOpen = this.onOpen.bind(this);
    this.changeColor = this.changeColor.bind(this);
  }

  async componentDidMount() {
    await Font.loadAsync({
      Roboto: require("native-base/Fonts/Roboto.ttf"),
      Roboto_medium: require("native-base/Fonts/Roboto_medium.ttf"),
      ...EvilIcons.font,
      ...Ionicons.font,
    });
    const connectionHost = await AsyncStorage.getItem(connectionHostKey);
    const connectionPort = await AsyncStorage.getItem(connectionPortKey);
    this.setState({
      isReady: true,
      host: connectionHost || "",
      port: connectionPort || "81",
    });
    if (connectionHost && connectionPort) {
      this.connect();
    }
    if (!connectionHost || !connectionPort) {
      Toast.show({
        text: "Host or Port not set",
        duration: 3000,
        type: "warning",
      });
    }
  }

  async saveSettings(host: string, port: string) {
    await AsyncStorage.setItem(connectionHostKey, host);
    await AsyncStorage.setItem(connectionPortKey, port);
    this.setState({ host, port });
  }

  onOpen() {
    this.setState({ state: "connected" });
    Toast.show({
      text: "Connected",
      type: "success",
    });
  }

  onMessage(e: WebSocketMessageEvent) {
    console.log(e);
    if (e.data.startsWith("CURR")) {
      const [, power, bri, r, g, b] = e.data.split(" ");
      console.log(
        power,
        bri,
        r,
        g,
        b,
        rgbToHex({
          r: parseInt(r, 10),
          g: parseInt(g, 10),
          b: parseInt(b, 10),
        }),
        toHsv(
          rgbToHex({
            r: parseInt(r, 10),
            g: parseInt(g, 10),
            b: parseInt(b, 10),
          })
        )
      );
      this.setState({
        power: parseInt(power, 10),
        bri: parseInt(bri, 10),
        color: toHsv(
          rgbToHex({
            r: parseInt(r, 10),
            g: parseInt(g, 10),
            b: parseInt(b, 10),
          })
        ),
      });
    } else if (e.data === "P_ON" || e.data === "P_OFF") {
      this.setState({ power: e.data === "P_ON" ? 1 : 0 });
    } else if (e.data.startsWith("BRI")) {
      const bri = parseInt(e.data.replace("BRI", ""), 10);
      this.setState({ bri });
    } else {
      Toast.show({
        text: `New message: ${e.data}`,
      });
    }
  }

  onClose() {
    this.setState({ state: "disconnected" });
    Toast.show({
      text: "Disconnected",
      type: "warning",
    });
  }

  connect() {
    const { host, port } = this.state;
    this.ws = new WebSocket(`ws://${host}:${port}`);
    this.ws.onopen = this.onOpen;
    this.ws.onmessage = this.onMessage;
    this.ws.onerror = (e) => {
      console.log(e);
      Toast.show({
        text: "Websocket error",
        type: "danger",
      });
    };
    this.ws.onclose = this.onClose;
  }

  changeState() {
    const { state } = this.state;
    if (state === "connected") {
      this.setState({ state: "disconnecting" });
      if (this.ws) {
        this.ws.close();
      } else {
        this.setState({ state: "disconnected" });
      }
    } else {
      this.setState({ state: "connecting" });
      this.connect();
    }
  }

  changePower() {
    if (this.ws) {
      this.ws.send(this.state.power ? "P_OFF" : "P_ON");
    }
  }

  changeColor(hsv: HsvColor) {
    const color = fromHsv(hsv);
    const rgb = hexToRgb(color);
    if (this.ws && rgb && this.state.state === "connected") {
      this.ws.send(`C ${rgb.r} ${rgb.g} ${rgb.b}`);
    }
    this.setState({ color: toHsv(color) });
  }

  render() {
    const {
      isReady,
      settingsModal,
      state,
      host,
      port,
      power,
      bri,
      color,
    } = this.state;
    if (!isReady) {
      return <AppLoading />;
    }

    return (
      <Root>
        <StatusBar style="auto" />
        <Settings
          state={settingsModal}
          host={host}
          port={port}
          onClose={(params) => {
            if (params && params.h !== "" && params.p !== "") {
              const { h, p } = params;
              this.saveSettings(h, p).catch(console.error);
            }
            this.setState({ settingsModal: false });
          }}
        />
        <Container>
          <Header>
            <Left>
              <Button
                disabled={host === "" || port === ""}
                success={state === "connected"}
                warning={state === "connecting"}
                danger={state === "disconnecting"}
                transparent
                onPress={this.changeState}
              >
                <Icon type="EvilIcons" name="link" />
              </Button>
            </Left>
            <Body>
              <Title>MoonLamp</Title>
            </Body>
            <Right>
              <Button
                disabled={state !== "disconnected"}
                transparent
                onPress={() => this.setState({ settingsModal: true })}
              >
                <Icon type="EvilIcons" name="gear" />
              </Button>
            </Right>
          </Header>
          <View
            style={{
              marginTop: 50,
              justifyContent: "center",
              alignItems: "center",
            }}
          >
            <TriangleColorPicker
              color={color}
              style={{ width: w, height: 250, marginBottom: 30 }}
              onColorChange={this.changeColor}
            />
            <Slider
              style={{ width: w, height: 30 }}
              minimumValue={0}
              maximumValue={255}
              value={bri}
              onSlidingComplete={(v) =>
                this.ws && this.ws.send(`BRI${v.toFixed(0)}`)
              }
            />
            <Button
              style={{ height: 60, width: 70, alignSelf: "center" }}
              disabled={state !== "connected"}
              transparent
              onPress={this.changePower}
            >
              <Icon
                name="power"
                style={{ fontSize: 50, color: power ? "green" : "red" }}
              />
            </Button>
          </View>
        </Container>
      </Root>
    );
  }
}
