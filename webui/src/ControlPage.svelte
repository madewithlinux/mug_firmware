<script lang="ts">
  import { MugState, postMugState } from "./lib";
  import { fixed } from "./input_format";

  export let remoteState: MugState;

  $: uptimeS = Math.floor((remoteState.millis / 1000) % 60)
    .toString()
    .padStart(2, "0");
  $: uptimeM = Math.floor((remoteState.millis / (1000 * 60)) % 60)
    .toString()
    .padStart(2, "0");
  $: uptimeH = Math.floor(remoteState.millis / (1000 * 60 * 60))
    .toString()
    .padStart(2, "0");

  const inputs: Record<string, HTMLInputElement> = {};
  let state = { ...remoteState };
  let prevState = { ...remoteState };
  const updateState = (nextState: MugState) => {
    for (const [key, value] of Object.entries(nextState)) {
      if (inputs[key] === document.activeElement) {
        continue;
      } else if (state[key] !== prevState[key]) {
        continue;
      } else if (state[key] !== value) {
        state[key] = value;
      }
    }
    prevState = nextState;
  };
  $: updateState(remoteState);

  let promise: Promise<any> | undefined = undefined;
  const submitMugState = () => {
    const newMugState = {
      ...remoteState,
      ...state,
    };
    promise = (async () => {
      state = await postMugState(newMugState);
    })();
  };
</script>

<form class="config" on:submit|preventDefault|stopPropagation|capture={submitMugState}>
  <span>uptime</span><span class="num">{uptimeH}:{uptimeM}:{uptimeS}</span>

  <span>heater state</span>
  <span class="num">{remoteState.is_heater_on ? "ON" : "OFF"} (level={remoteState.pwm_output})</span>

  <span>current_temp_f</span><span class="num">{remoteState.current_temp_f.toFixed(4)}</span>

  <label for="target_temp_f">target_temp</label>
  <input id="target_temp_f" type="number" bind:value={state.target_temp_f} use:fixed={2} />
  <label for="threshold_temp_f">threshold_temp</label>
  <input id="threshold_temp_f" type="number" bind:value={state.threshold_temp_f} use:fixed={2} />
  <label for="temp_hot">temp_hot</label>
  <input id="temp_hot" type="number" bind:value={state.temp_hot} use:fixed={1} />
  <label for="temp_cold">temp_cold</label>
  <input id="temp_cold" type="number" bind:value={state.temp_cold} use:fixed={1} />

  <label for="refresh_interval">refresh_interval</label><input
    id="refresh_interval"
    type="number"
    bind:value={state.refresh_interval}
  />

  <label for="kp">kp</label><input id="kp" type="number" step="0.1" bind:value={state.kp} use:fixed={2} />
  <label for="ki">ki</label><input id="ki" type="number" step="0.1" bind:value={state.ki} use:fixed={2} />
  <label for="kd">kd</label><input id="kd" type="number" step="0.1" bind:value={state.kd} use:fixed={2} />
  <label for="max_pid_output">max_pid_output</label><input id="max_pid_output" type="number" bind:value={state.max_pid_output} />

  <!-- <span>output</span><span class="num">{remoteState.output}</span> -->

  <label for="led_brightness">led_brightness</label>
  <input id="led_brightness" type="number" min="0" max="255" bind:value={state.led_brightness} />

  <button type="button" on:click|stopPropagation|preventDefault={() => (state = remoteState)}>reset</button>
  <button type="submit" on:click|stopPropagation|preventDefault={submitMugState}>save</button>
</form>

{#if promise}
{#await promise}
...loading
{:catch error}
<p style="color: red">error: {error.message}</p>
{/await}
{/if}

<pre>{JSON.stringify(remoteState, undefined, " ")}</pre>

<style>
  .config {
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 10px;
  }
  @media (min-width: 600px) {
    .config {
      max-width: max-content;
    }
  }

  .num {
    justify-self: end;
  }

  .config,
  label,
  input {
    font-size: 1.2rem;
  }
  label {
    justify-self: stretch;
    align-self: stretch;
  }
  /* select, */
  input {
    justify-self: end;
  }
  input[type="number"] {
    width: 140px;
    text-align: end;
  }
  /* select {
    width: min-content;
  } */
  /* input[type="submit"], */
  button {
    margin: 5px;
    padding: 20px 10px;
    font-size: 1.2rem;
  }
</style>
