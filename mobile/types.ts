export type ConnectionState =
  | "connecting"
  | "connected"
  | "disconnecting"
  | "disconnected";

export type AppState = {
  host: string;
  port: string;
  power: number;
  bri: number;
  state: ConnectionState;
  color: { h: number; s: number; v: number }
  settingsModal: boolean;
  isReady: boolean;
};

export type AppProps = {};

export type SettingsProps = {
  state: boolean;
  host: string;
  port: string;
  onClose: (params?: { h: string; p: string }) => void;
};
