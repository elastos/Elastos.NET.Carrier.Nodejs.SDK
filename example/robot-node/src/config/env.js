import * as dotenv from 'dotenv';
import * as path from 'path';

const configFilePath = path.resolve(__dirname, '../../.env');

export default dotenv.config({ path: configFilePath }).parsed;