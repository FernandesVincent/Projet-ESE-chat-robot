const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');

const options = {
  path: '/dev/serial0',
  baudRate: 115200,
  dataBits: 8,
  parity: 'none',
  stopBits : 1
}

let TOF1;
let TOF2;
let TOF3;
let TOF4;
let TOF5;
let TOF6;
let LIDAR_D;
let LIDAR_THETA;
let speed;
let accel;
let batt; 
let role;
let cat_as_not;
let mouse_as_not;

const port = new SerialPort(options);

const parser = port.pipe(new ReadlineParser({ delimiter: '\n' }));

parser.on('data', data => {
  console.log('Reçu depuis la Nucleo:', data);
  console.log(typeof data);

  const dataObj = JSON.parse(data);

  TOF1 = dataObj.TOF1;
  TOF2 = dataObj.TOF2;
  TOF3 = dataObj.TOF3;
  TOF4 = dataObj.TOF4;
  TOF5 = dataObj.TOF5;
  TOF6 = dataObj.TOF6;
  LIDAR_D = dataObj.LIDAR_D;
  LIDAR_THETA = dataObj.LIDAR_THETA;
  speed = dataObj.speed;
  accel = dataObj.accel;
  batt = dataObj.batt;
  role = dataObj.role;
  cat_as_not = dataObj.cat_as_not;
  mouse_as_not = dataObj.mouse_as_not;

  console.log("TOF1 =", TOF1, "Speed =", speed, "Battery =", batt);
});


port.on('open', () => {
  console.log('Port série ouvert');
  port.write('Hello Nucleo!\n');
});


