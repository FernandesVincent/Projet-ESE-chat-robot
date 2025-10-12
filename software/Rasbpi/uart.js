const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');

const options = {
  path: '/dev/serial0',
  baudRate: 115200,
  dataBits: 8,
  parity: 'none',
  stopBits : 1
}

const port = new SerialPort(options);

const parser = port.pipe(new ReadlineParser({ delimiter: '\n' }));

parser.on('data', data => {
  console.log('Reçu depuis la Nucleo:', data);
  console.log(typeof data);

  const dataObj = JSON.parse(data);

  const TOF1 = dataObj.TOF1;
  const TOF2 = dataObj.TOF2;
  const TOF3 = dataObj.TOF3;
  const TOF4 = dataObj.TOF4;
  const TOF5 = dataObj.TOF5;
  const TOF6 = dataObj.TOF6;
  const LIDAR_D = dataObj.LIDAR_D;
  const LIDAR_THETA = dataObj.LIDAR_THETA;
  const speed = dataObj.speed;
  const accel = dataObj.accel;
  const batt = dataObj.batt;
  const role = dataObj.role;
  const cat_as_not = dataObj.cat_as_not;
  const mouse_as_not = dataObj.mouse_as_not;

  console.log("TOF1 =", TOF1, "Speed =", speed, "Battery =", batt);
});



port.on('open', () => {
  console.log('Port série ouvert');
  port.write('Hello Nucleo!\n');
});


