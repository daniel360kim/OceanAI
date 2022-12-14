/**
 * To strictly type all accessors and writers, remove
 *
 * [messageID: string]: any
 *
 * And replace with your entire state shape after codecs have decoded them.
 */
declare global {
  interface ElectricUIDeveloperState {
    [messageID: string]: any

    // Example messageID typings
    orixDS: number
    oriyDS: number
    orizDS: number
  }
  interface ElectricUIDeviceMetadataState {
    name: string
  }
}

export type Angles3D = {
  x: number
  y: number
  z: number

}


