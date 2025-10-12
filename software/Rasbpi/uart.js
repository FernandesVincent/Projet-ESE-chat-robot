const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');

const options = {
  path: '/dev/serial0',
  baudRate: 115200,
  dataBits: 8,
  parity: 'None',
  stopBits : 1
}


const port = new SerialPort(options);

if (port.connected) {
  console.log("Nucleo connected");
} else {
  console.log("Nucleo not connected");
}

const parser = port.pipe(new ReadlineParser({ delimiter: '\n' }));

parser.on('data', data => {
console.log('Reçu depuis la Nucleo:', data);
});

port.on('open', () => {
console.log('Port série ouvert');
port.write('Hello Nucleo!\n');
});