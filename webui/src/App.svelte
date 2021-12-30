<script lang="ts">
  import ControlPage from "./ControlPage.svelte";
  import { fetchMugState, MugState } from "./lib";

  let state: MugState | undefined;

  async function refreshState() {
    state = await fetchMugState();
    return state;
  }

  let promise = refreshState();
  function handleClick() {
    promise = refreshState();
  }

  let source: EventSource;
  if (!!window.EventSource) {
    source = new EventSource("http://192.168.1.173/events");

    source.onopen = (e) => {
      console.log("Events Connected");
    };

    source.onmessage = (e) => {
      console.log(e);
    };

    source.addEventListener(
      "state",
      (e: MessageEvent) => {
        state = JSON.parse(e.data);
      },
      false
    );
  }
</script>

<main>
  <h1>Mug</h1>

  <p>
    <!-- <button on:click={handleClick}> refresh </button> -->
    <a href="/">refresh</a>
    {#await promise}
      ...loading
    {:then state}
      <!-- <p>state: {state}</p> -->
    {:catch error}
      <p style="color: red">error: {error.message}</p>
    {/await}
  </p>

  {#if state}
    <ControlPage remoteState={state} />
  {/if}
</main>

<style>
  :global(body) {
    /* color: #333; */
    margin: 0 20px;
    padding: 8px;
    box-sizing: border-box;
    font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Oxygen-Sans, Ubuntu, Cantarell,
      "Helvetica Neue", sans-serif;
    font-size: 1.2rem;
  }

  main {
    /* padding: 1em; */
    padding-left: 1em;
    padding-right: 1em;
    /* max-width: 240px; */
    margin: 0 auto;
  }

  @media (max-width: 1000px) {
    :global(body) {
      padding: 0;
    }
    main {
      max-width: none;
      padding-left: 10px;
      padding-right: 10px;
    }
  }
</style>
