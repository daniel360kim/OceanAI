import 'source-map-support/register'

import { deviceManager } from './config'
import { setupProxyAndDebugInterface } from '@electricui/components-desktop-blueprint'
import { setupTransportWindow } from '@electricui/utility-electron'
import {
  ElectronIPCRemoteQueryExecutor,
  QueryableMessageIDProvider,
  DataSource,
  PersistenceEngineMemory,
  PersistenceEnginePassthrough,
} from '@electricui/core-timeseries'

import './styles.css'

const root = document.createElement('div')
document.body.appendChild(root)

const hotReloadHandler = setupProxyAndDebugInterface(root, deviceManager)
setupTransportWindow()


const remoteQueryExecutor = new ElectronIPCRemoteQueryExecutor()
const queryableMessageIDProvider = new QueryableMessageIDProvider(
  deviceManager,
  remoteQueryExecutor,
)

if (module.hot) {
  module.hot.accept('./config', () => hotReloadHandler(root, deviceManager))
}


queryableMessageIDProvider.setPersistenceEngineFactory(
  'lt',

  (dataSource: DataSource) => {
    // Use the in-memory persistence engine with a ceiling of 20k points
    return new PersistenceEngineMemory(20_000)
  },
)
queryableMessageIDProvider.setPersistenceEngineFactory(
  'v',

  (dataSource: DataSource) => {
    // Use the in-memory persistence engine with a ceiling of 20k points
    return new PersistenceEngineMemory(20_000)
  },
)
queryableMessageIDProvider.setPersistenceEngineFactory(
  'sst',

  (dataSource: DataSource) => {
    // Use the in-memory persistence engine with a ceiling of 20k points
    return new PersistenceEngineMemory(20_000)
  },
)

queryableMessageIDProvider.setPersistenceEngineFactory(
  'it',

  (dataSource: DataSource) => {
    // Use the in-memory persistence engine with a ceiling of 20k points
    return new PersistenceEngineMemory(20_000)
  },
)

queryableMessageIDProvider.setPersistenceEngineFactory(
  'xd',

  (dataSource: DataSource) => {
    // Use the in-memory persistence engine with a ceiling of 20k points
    return new PersistenceEngineMemory(20_000)
  },
)
queryableMessageIDProvider.setPersistenceEngineFactory(
  'yd',

  (dataSource: DataSource) => {
    // Use the in-memory persistence engine with a ceiling of 20k points
    return new PersistenceEngineMemory(20_000)
  },
)
queryableMessageIDProvider.setPersistenceEngineFactory(
  'zd',

  (dataSource: DataSource) => {
    // Use the in-memory persistence engine with a ceiling of 20k points
    return new PersistenceEngineMemory(20_000)
  },
)
queryableMessageIDProvider.setPersistenceEngineFactory(
  'gd',

  (dataSource: DataSource) => {
    // Use the in-memory persistence engine with a ceiling of 20k points
    return new PersistenceEngineMemory(20_000)
  },
)
queryableMessageIDProvider.setPersistenceEngineFactory(
  'ad',

  (dataSource: DataSource) => {
    // Use the in-memory persistence engine with a ceiling of 20k points
    return new PersistenceEngineMemory(20_000)
  },
)

queryableMessageIDProvider.setPersistenceEngineFactory(
  'sp',

  (dataSource: DataSource) => {
    // Use the in-memory persistence engine with a ceiling of 20k points
    return new PersistenceEngineMemory(20_000)
  },
)

queryableMessageIDProvider.setPersistenceEngineFactory(
  'st',

  (dataSource: DataSource) => {
    // Use the in-memory persistence engine with a ceiling of 20k points
    return new PersistenceEngineMemory(20_000)
  },
)
queryableMessageIDProvider.setPersistenceEngineFactory(
  'ss',

  (dataSource: DataSource) => {
    // Use the in-memory persistence engine with a ceiling of 20k points
    return new PersistenceEngineMemory(20_000)
  },
)
queryableMessageIDProvider.setPersistenceEngineFactory(
  'sa',

  (dataSource: DataSource) => {
    // Use the in-memory persistence engine with a ceiling of 20k points
    return new PersistenceEngineMemory(20_000)
  },
)
queryableMessageIDProvider.setPersistenceEngineFactory(
  'ssc',

  (dataSource: DataSource) => {
    // Use the in-memory persistence engine with a ceiling of 20k points
    return new PersistenceEngineMemory(20_000)
  },
)
queryableMessageIDProvider.setPersistenceEngineFactory(
  'sc',

  (dataSource: DataSource) => {
    // Use the in-memory persistence engine with a ceiling of 20k points
    return new PersistenceEngineMemory(20_000)
  },
)
queryableMessageIDProvider.setPersistenceEngineFactory(
  'ac',

  (dataSource: DataSource) => {
    // Use the in-memory persistence engine with a ceiling of 20k points
    return new PersistenceEngineMemory(20_000)
  },
)

queryableMessageIDProvider.setPersistenceEngineFactory(
  'hd',

  (dataSource: DataSource) => {
    // Use the in-memory persistence engine with a ceiling of 20k points
    return new PersistenceEngineMemory(20_000)
  },
)

queryableMessageIDProvider.setPersistenceEngineFactory(
  'ld',

  (dataSource: DataSource) => {
    // Use the in-memory persistence engine with a ceiling of 20k points
    return new PersistenceEngineMemory(20_000)
  },
)

queryableMessageIDProvider.setPersistenceEngineFactory(
  'hr',

  (dataSource: DataSource) => {
    // Use the in-memory persistence engine with a ceiling of 20k points
    return new PersistenceEngineMemory(20_000)
  },
)

queryableMessageIDProvider.setPersistenceEngineFactory(
  'hp',

  (dataSource: DataSource) => {
    // Use the in-memory persistence engine with a ceiling of 20k points
    return new PersistenceEngineMemory(20_000)
  },
)
