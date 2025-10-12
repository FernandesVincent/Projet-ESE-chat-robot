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

// let dataRaw = '';
parser.on('data', data => {
  console.log('Reçu depuis la Nucleo:', data);
  console.log(typeof data);
  // dataRaw = data.toString();
});

port.on('open', () => {
  console.log('Port série ouvert');
  port.write('Hello Nucleo!\n');
});


