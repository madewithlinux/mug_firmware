// input_format.ts

export const fixed = (node: HTMLInputElement, fractionDigits: number) => {
  const setNumberFormat = () => {
    if (node) {
      node.value = node.valueAsNumber.toFixed(fractionDigits);
    }
  };
  console.log("fixed", node);
  setNumberFormat();
  node.addEventListener("change", setNumberFormat);
  return {
    destroy() {
      node.removeEventListener("change", setNumberFormat);
    },
  };
};
