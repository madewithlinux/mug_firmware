export interface MugState {
  is_heater_on: boolean;
  pwm_output: number;
  current_temp_f: number;
  target_temp_f: number;
  threshold_temp_f: number;
  temp_hot: number;
  temp_cold: number;
  refresh_interval: number;
  kp: number;
  ki: number;
  kd: number;
  output: number;
  led_brightness: number;
  millis: number;
  heap: number;
}

export async function fetchMugState(): Promise<MugState> {
  const res = await fetch(`http://192.168.1.173/api/state`);
  const text = await res.json();
  if (res.ok) {
    return text;
  } else {
    throw new Error(text);
  }
}

export async function postMugState(newState: MugState): Promise<MugState> {
  const res = await fetch(`http://192.168.1.173/api/state`, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(newState),
  });
  if (res.ok) {
    return await fetchMugState();
  } else {
    throw new Error(await res.text());
  }
}
