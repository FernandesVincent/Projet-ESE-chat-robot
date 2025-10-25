let controllerIndex = null;
let controllerisconnected = 0;
let gamepad;

let data = [
  [0, 0],
  [1, 0],
  [2, 0],
  [3, 0],
  [4, 0],
  [5, 0],
  [6, 0],
  [7, 0],
  [12, 0],
  [13, 0],
  [14, 0],
  [15, 0],
];

window.addEventListener("gamepadconnected", (event) => {
  const gamepad = event.gamepad;
  controllerIndex = gamepad.index;
  console.log("connected");

});

window.addEventListener("gamepaddisconnected", (event) => {
  controllerIndex = null;
  console.log("disconnected");
});

function connection() {
  return new Promise((resolve) => {
    const interval = setInterval(() => {
      if (controllerIndex !== null) {
        controllerisconnected = 1;
        clearInterval(interval);
        resolve("Controller connected");
      }
    }, 10);
  });
}

let checkButtons = () => {
  if (controllerisconnected === 1) {
    gamepad = navigator.getGamepads()[controllerIndex]; //gamepad static donc besoin de rafraichir à chaque fois
    let buttons = gamepad.buttons;
    for (let i = 0; i < buttons.length; i++) {
      const button = buttons[i];
      if (button.value > 0) {
        if(data[i]){
          console.log(button);
          console.log("numéro du bouton cliqué", i);
          console.log(data[i]);
          data[i][1] = 1;
          console.log(data[i]);
        }
      }
    }
  }
};

let clearAllButonns = () => {
  if (controllerisconnected === 1) {
    gamepad = navigator.getGamepads()[controllerIndex]; //gamepad static donc besoin de rafraichir à chaque fois
    let buttons = gamepad.buttons;
    for(let i = 0; i<buttons.length; i++){
      if(data[i]){
        // console.log(data[i])
        data[i][1] = 0;
      }
    }
  }
}

let previousData = data.map(row => [...row]); // copie initiale

let compareData = () => {
  if (controllerisconnected !== 1) return;

  for (let i = 0; i < data.length; i++) {
    if (data[i][1] !== previousData[i][1]) {
      previousData = data.map(row => [...row]); // actualiser la copie
      return "ReadyToSend";
    }
  }
};

let dataToSend;

connection()
  .then(() => {
    checkButtons();
    setInterval(() => {
      checkButtons();
      if(compareData() === "ReadyToSend"){
        dataToSend = data.map(row => [...row]); 
        console.log("dataToSend :", dataToSend);
      }
      setTimeout(() => {
        clearAllButonns();
      }, 100);
    }, 150);
  })

  