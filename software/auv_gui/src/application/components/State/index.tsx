import React from 'react'

import darkOn from './dark-on.png'
import darkOff from './dark-off.png'
import lightOn from './light-on.png'
import lightOff from './light-off.png'
import { useDarkMode, ImageSwitcher } from '@electricui/components-desktop'
import { useHardwareState } from '@electricui/components-core'
import CSS from 'csstype'

type LightBulbProps = {
  style?: React.CSSProperties
  containerStyle?: React.CSSProperties
  width?: CSS.Properties['width']
  height?: CSS.Properties['height']
}

export const LightBulb = (props: LightBulbProps) => {
  const isOn = useHardwareState('led_state')
  const isDarkMode = useDarkMode()

  const images = [darkOn, darkOff, lightOn, lightOff]

  let image = 0
  if (isDarkMode) {
    if (isOn) {
      image = 0
    } else {
      image = 1
    }
  } else {
    if (isOn) {
      image = 2
    } else {
      image = 3
    }
  }

  return (
    <ImageSwitcher
      images={images}
      active={image}
      style={props.style}
      containerStyle={props.containerStyle}
      width={props.width}
      height={props.height}
    />
  )
}
