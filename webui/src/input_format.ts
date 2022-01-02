// input_format.ts

export const fixed = (node: HTMLInputElement, fractionDigits: number) => {
  const setNumberFormat = () => {
    if (node) {
      node.value = node.valueAsNumber.toFixed(fractionDigits);
    }
  };
  setNumberFormat();
  node.addEventListener("change", setNumberFormat);
  return {
    destroy() {
      node.removeEventListener("change", setNumberFormat);
    },
  };
};
