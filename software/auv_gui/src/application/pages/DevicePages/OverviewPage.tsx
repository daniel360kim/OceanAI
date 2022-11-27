import { 
  ChartContainer,
  LineChart,
  RealTimeDomain,
  TimeAxis,
  VerticalAxis,
} from '@electricui/components-desktop-charts'

import { Card } from '@blueprintjs/core'
import { Composition } from 'atomic-layout'
import { IntervalRequester } from '@electricui/components-core'
import { LightBulb } from '../../components/LightBulb'
import { MessageDataSource } from '@electricui/core-timeseries'
import React from 'react'
import { RouteComponentProps } from '@reach/router'
import { Slider } from '@electricui/components-desktop-blueprint'
import { Printer } from '@electricui/components-desktop'

const layoutDescription = `
  Chart Chart
  Light Slider
`

//Data Inputs
 
const loop_timeDS = new MessageDataSource("loop_time")
const sys_stateDS = new MessageDataSource("system_state")

const filt_voltageDS = new MessageDataSource("filt_voltage")
//const clock_speedDS = new MessageDataSource("clock_speed");
//const internal_tempDS = new MessageDataSource("internal_temp")

//const bmp_presDS = new MessageDataSource("raw_bmp_pressure")
//const bmp_tempDS = new MessageDataSource("raw_bmp_temperature")


//const tdsDS = new MessageDataSource("tds")
const ext_presDS = new MessageDataSource("ext_pres")
const ext_tempDS = new MessageDataSource("ext_temp")

/*
const limit_stateDS = new MessageDataSource("limit_state")
const homedDS = new MessageDataSource("homed")
const step_posDS = new MessageDataSource("pos")
const step_pos_mmDS = new MessageDataSource("pos_mm")
const step_tarDS = new MessageDataSource("target_pos")
const step_tar_mmDS = new MessageDataSource("target_pos_mm")
const step_speedDS = new MessageDataSource("step_speed")
const step_accelDS = new MessageDataSource("step_accel")
const step_max_speedDS = new MessageDataSource("step_max_speed")

*/
export const OverviewPage = (props: RouteComponentProps) => {
  return (
    <React.Fragment>
      <IntervalRequester
        variables={['time_ms', 'loop_time', 'system_state', 'filt_voltage', 'ext_pres', 'ext_temp',]}
        interval={50}
      />

      <Composition areas={layoutDescription} gap={10} autoCols="1fr">
        {Areas => (
          <React.Fragment>

          </React.Fragment>
        )}
      </Composition>
    </React.Fragment>
  )
}
