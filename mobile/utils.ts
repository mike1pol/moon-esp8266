type HexToRgb = {
  r: number;
  g: number;
  b: number;
};

export const hexToRgb = (hex: string): HexToRgb | null => {
  const result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
  return result
    ? {
      r: parseInt(result[1], 16),
      g: parseInt(result[2], 16),
      b: parseInt(result[3], 16),
    }
    : null;
};

export const rgbToHex = ({r, g, b}: HexToRgb): string => {
  return `#${r.toString(16)}${g.toString(16)}${b.toString(16)}`;
}
