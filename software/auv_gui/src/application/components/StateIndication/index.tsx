import React from 'react'

import { Callout } from '@blueprintjs/core'
import { useHardwareState } from '@electricui/components-core'
import CSS from 'csstype'

type StateIndicatorProps = {
  style?: React.CSSProperties
  containerStyle?: React.CSSProperties
  width?: CSS.Properties['width']
  height?: CSS.Properties['height']
}
export const StateIndicator = (props: StateIndicatorProps) => {
  const state = useHardwareState('system_state')
  if (state == 0) {
    return (
      <Callout title="Initialization" intent="primary" icon="pulse"></Callout>
    )
  } else if (state == 1) {
    return <Callout title="Error" intent="danger" icon="error">
        System is in an error. Please restart.
    </Callout>
  } else if (state == 2) {
    return <Callout title="Idle" intent="primary" icon="tick">
        Waiting for input.
    </Callout>
  } else if (state == 3) {
    return <Callout title="Diving" intent="primary" icon="pulse">
        Next Up: Resurfacing
    </Callout>
  } else if (state == 4) {
    return <Callout title="Resurfacing" intent="primary">
        Next Up: Surfaced
    </Callout>
  } else if (state == 5) {
    return <Callout title="Surfaced" intent="primary">
        Next Up: Diving
    </Callout>
  } else if (state == 6) {
    return <Callout title="Calibrating" intent="danger">
        Next Up: Resurfacing
    </Callout>
  } else {
    return <Callout title="Unknown" intent="danger" icon="error"></Callout>
  }
}
