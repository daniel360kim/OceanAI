import {
  ChartContainer,
  LineChart,
  RealTimeDomain,
  TimeAxis,
  VerticalAxis,
  HorizontalAxis,
  ZoomBrush,
  TimeSlicedLineChart,
  RealTimeSlicingDomain,
  Fog,
} from '@electricui/components-desktop-charts'

import React from 'react'
import { MessageDataSource } from '@electricui/core-timeseries'
import { RouteComponentProps } from '@reach/router'
import { Card } from '@blueprintjs/core'
import { Composition } from 'atomic-layout'
import { Printer } from '@electricui/components-desktop'
import { IntervalRequester } from '@electricui/components-core'
import IMUmodel from '../../components/xsens-mti300/xsens-mti300.glb'
import { LightBulb } from '../../components/LightBulb'
import { time } from 'console'
import { HTMLTable } from '@blueprintjs/core'
import { Connections } from '@electricui/components-desktop-blueprint'
import {
  Environment,
  GLTF,
  OrbitControls,
  ControlledGroup,
} from '@electricui/components-desktop-three'

/*
const vxDS = new MessageDataSource("vel_x")
const vyDS = new MessageDataSource("vel_y")
const vzDS = new MessageDataSource("vel_z")
const pxDS = new MessageDataSource("pos_x")
const pyDS = new MessageDataSource("pos_y")
const pzDS = new MessageDataSource("pos_z")
*/

GLTF.preload(IMUmodel)
const timeDS = new MessageDataSource('time_ms')
const axDS = new MessageDataSource('wfacc_x')
const ayDS = new MessageDataSource('wfacc_y')
const azDS = new MessageDataSource('wfacc_z')
const gxDS = new MessageDataSource('rgyr_x')
const gyDS = new MessageDataSource('rgyr_y')
const gzDS = new MessageDataSource('rgyr_z')
const mxDs = new MessageDataSource('mag_x')
const myDs = new MessageDataSource('mag_y')
const mzDs = new MessageDataSource('mag_z')
const rxDS = new MessageDataSource('rel_x')
const ryDS = new MessageDataSource('rel_y')
const rzDS = new MessageDataSource('rel_z')

const navigationLayoutDescription = `
    IMUNumbers Chart
    Light Other1 
`

const lerp = (x: number, y: number, a: number) => x * (1 - a) + y * a
const clamp = (a: number, min = 0, max = 1) => Math.min(max, Math.max(min, a))
const invlerp = (x: number, y: number, a: number) => clamp((a - x) / (y - x))



export const NavigationPage = (props: RouteComponentProps) => {
  return (
    <React.Fragment>
      <IntervalRequester
        variables={[
          'wfacc_x',
          'wfacc_y',
          'wfacc_z',
          'rgyr_x',
          'rgyr_y',
          'rgyr_z',
          'mag_x',
          'mag_y',
          'mag_z',
          'rel_x',
          'rel_y',
          'rel_z',
        ]}
        interval={50}
      />

      <Composition areas={navigationLayoutDescription} gap={10} autoCols="1fr">
        {Areas => (
          <React.Fragment>


            <Areas.Chart>
              <Card>
                <div style={{ textAlign: 'left', marginBottom: '1em' }}>
                  <h1>IMU Data</h1>
                </div>
                <div style={{ textAlign: 'center', marginBottom: '1em' }}>
                  <b>Orientation</b>
                </div>
                <ChartContainer>
                  <LineChart dataSource={rxDS} />
                  <LineChart dataSource={ryDS} />
                  <LineChart dataSource={rzDS} />
                  <RealTimeDomain window={10000} />
                  <TimeAxis label="Time (s)" />
                  <VerticalAxis label="Orientation (deg)" />
                  <ZoomBrush />
                </ChartContainer>

                <div style={{ textAlign: 'center', marginBottom: '1em' }}>
                  <b>Angular Rates</b>
                </div>
                <ChartContainer>
                  <LineChart dataSource={gxDS} />
                  <LineChart dataSource={gyDS} />
                  <LineChart dataSource={gzDS} />
                  <RealTimeDomain window={10000} />
                  <TimeAxis label="Time (s)" />
                  <VerticalAxis label="Angular Rate (rad/s)" />
                  <ZoomBrush />
                </ChartContainer>

                <div style={{ textAlign: 'center', marginBottom: '1em' }}>
                  <b>Acceleration</b>
                </div>
                <ChartContainer>
                  <LineChart dataSource={axDS} />
                  <LineChart dataSource={ayDS} />
                  <LineChart dataSource={azDS} />
                  <RealTimeDomain window={10000} />
                  <TimeAxis label="Time (s)" />
                  <VerticalAxis label="Acceleration (m/s/s)" />
                  <ZoomBrush />
                </ChartContainer>
              </Card>
            </Areas.Chart>
          </React.Fragment>
        )}
      </Composition>
    </React.Fragment>
  )
}
