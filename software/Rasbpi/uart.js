import { SerialPort } from 'serialport';
import { ReadlineParser } from '@serialport/parser-readline';

const port = new SerialPort({
path: '/dev/serial0',
baudRate: 115200
});

const parser = port.pipe(new ReadlineParser({ delimiter: '\n' }));

parser.on('data', data => {
console.log('Reçu depuis la Nucleo:', data);
});

port.on('open', () => {
console.log('Port série ouvert');
port.write('Hello Nucleo!\n');
});