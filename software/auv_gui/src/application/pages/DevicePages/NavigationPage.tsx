import {
  ChartContainer,
  LineChart,
  RealTimeDomain,
  TimeAxis,
  VerticalAxis,
  HorizontalAxis,
  TimeSlicedLineChart,
  RealTimeSlicingDomain,
  Fog,
} from '@electricui/components-desktop-charts'

import {
  Environment,
  ControlledGroup,
  GLTF,
  OrbitControls,
} from '@electricui/components-desktop-three'

import IMUModel from '../../components/xsens-mti300/xsens-mti300.glb'

GLTF.preload(IMUModel)

import { Card, Colors } from '@blueprintjs/core'
import { Composition } from 'atomic-layout'

import { MessageDataSource } from '@electricui/core-timeseries'
import React from 'react'
import { RouteComponentProps } from '@reach/router'
import { Printer } from '@electricui/components-desktop'

const rxDS = new MessageDataSource<number>('xd')
const ryDS = new MessageDataSource<number>('yd')
const rzDS = new MessageDataSource<number>('zd')

const accDS = new MessageDataSource('ad')
const gyrDS = new MessageDataSource('gd')
const magDS = new MessageDataSource('md')

const navigationLayoutDescription = `
    Chart Numeric 
    Chart TimeSlice
    Chart Model
`

export const NavigationPage = (props: RouteComponentProps) => {
  return (
    <React.Fragment>
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
                </ChartContainer>

                <div style={{ textAlign: 'center', marginBottom: '1em' }}>
                  <b>Angular Rates</b>
                </div>
                <ChartContainer>
                  <LineChart
                    dataSource={gyrDS}
                    accessor={(data, time) => data[0]}
                    color={Colors.RED5}
                  />
                  <LineChart
                    dataSource={gyrDS}
                    accessor={(data, time) => data[1]}
                    color={Colors.GREEN5}
                  />
                  <LineChart
                    dataSource={gyrDS}
                    accessor={(data, time) => data[2]}
                    color={Colors.BLUE5}
                  />

                  <RealTimeDomain window={10000} />
                  <TimeAxis label="Time (s)" />
                  <VerticalAxis label="Angular Rate (rad/s)" />
                </ChartContainer>

                <div style={{ textAlign: 'center', marginBottom: '1em' }}>
                  <b>Acceleration</b>
                </div>
                <ChartContainer>
                  <LineChart
                    dataSource={accDS}
                    accessor={(data, time) => data[0]}
                    color={Colors.RED5}
                  />
                  <LineChart
                    dataSource={accDS}
                    accessor={(data, time) => data[1]}
                    color={Colors.GREEN5}
                  />
                  <LineChart
                    dataSource={accDS}
                    accessor={(data, time) => data[2]}
                    color={Colors.BLUE5}
                  />
                  <RealTimeDomain window={10000} />
                  <TimeAxis label="Time (s)" />
                  <VerticalAxis label="Acceleration (m/s/s)" />
                </ChartContainer>

                <div style={{ textAlign: 'center', marginBottom: '1em' }}>
                  <b>Magnetometer</b>
                </div>

                <ChartContainer>
                  <LineChart
                    dataSource={magDS}
                    accessor={(data, time) => data[0]}
                    color={Colors.RED5}
                  />
                  <LineChart
                    dataSource={magDS}
                    accessor={(data, time) => data[1]}
                    color={Colors.GREEN5}
                  />
                  <LineChart
                    dataSource={magDS}
                    accessor={(data, time) => data[2]}
                    color={Colors.BLUE5}
                  />
                  <RealTimeDomain window={10000} />
                  <TimeAxis label="Time (s)" />
                  <VerticalAxis label="Magnetometer (uTesla)" />
                </ChartContainer>
              </Card>
            </Areas.Chart>
            <Areas.Numeric>
              <Card>
                <h2>IMU</h2>

                <b>aX: </b>
                <Printer
                  accessor={state => state.ad[0]}
                  precision={3}
                  style={{
                    width: '3.5em',
                    height: '1.05em',
                    contain: 'strict',
                    textAlign: 'left',
                    display: 'inline-block',
                    overflow: 'hidden',
                    fontSize: '1.1em',
                  }}
                />

                <b>aY: </b>
                <Printer
                  accessor={state => state.ad[1]}
                  precision={3}
                  style={{
                    width: '3.5em',
                    height: '1.05em',
                    contain: 'strict',
                    textAlign: 'left',
                    display: 'inline-block',
                    overflow: 'hidden',
                    fontSize: '1.1em',
                  }}
                />

                <b>aZ: </b>
                <Printer
                  accessor={state => state.ad[2]}
                  precision={3}
                  style={{
                    width: '3.5em',
                    height: '1.05em',
                    contain: 'strict',
                    textAlign: 'left',
                    display: 'inline-block',
                    overflow: 'hidden',
                    fontSize: '1.1em',
                  }}
                />
                <br></br>

                <b>gX: </b>
                <Printer
                  accessor={state => state.gd[0]}
                  precision={3}
                  style={{
                    width: '3.5em',
                    height: '1.05em',
                    contain: 'strict',
                    textAlign: 'left',
                    display: 'inline-block',
                    overflow: 'hidden',
                    fontSize: '1.1em',
                  }}
                />

                <b>gY: </b>
                <Printer
                  accessor={state => state.gd[1]}
                  precision={3}
                  style={{
                    width: '3.5em',
                    height: '1.05em',
                    contain: 'strict',
                    textAlign: 'left',
                    display: 'inline-block',
                    overflow: 'hidden',
                    fontSize: '1.1em',
                  }}
                />

                <b>gZ: </b>
                <Printer
                  accessor={state => state.gd[2]}
                  precision={3}
                  style={{
                    width: '3.5em',
                    height: '1.05em',
                    contain: 'strict',
                    textAlign: 'left',
                    display: 'inline-block',
                    overflow: 'hidden',
                    fontSize: '1.1em',
                  }}
                />

                <br></br>

                <b>mX: </b>
                <Printer
                  accessor={state => state.md[0]}
                  precision={3}
                  style={{
                    width: '3.5em',
                    height: '1.05em',
                    contain: 'strict',
                    textAlign: 'left',
                    display: 'inline-block',
                    overflow: 'hidden',
                    fontSize: '1.1em',
                  }}
                />

                <b>mY: </b>
                <Printer
                  accessor={state => state.md[1]}
                  precision={3}
                  style={{
                    width: '3.5em',
                    height: '1.05em',
                    contain: 'strict',
                    textAlign: 'left',
                    display: 'inline-block',
                    overflow: 'hidden',
                    fontSize: '1.1em',
                  }}
                />

                <b>mZ: </b>
                <Printer
                  accessor={state => state.md[2]}
                  precision={3}
                  style={{
                    width: '3.5em',
                    height: '1.05em',
                    contain: 'strict',
                    textAlign: 'left',
                    display: 'inline-block',
                    overflow: 'hidden',
                    fontSize: '1.1em',
                  }}
                />

                <h2>Localization</h2>

                <b>X: </b>
                <Printer
                  accessor="xd"
                  precision={3}
                  style={{
                    width: '4em',
                    height: '1.05em',
                    contain: 'strict',
                    textAlign: 'left',
                    display: 'inline-block',
                    overflow: 'hidden',
                    fontSize: '1.1em',
                  }}
                />

                <b>Y: </b>
                <Printer
                  accessor="yd"
                  precision={3}
                  style={{
                    width: '4em',
                    height: '1.05em',
                    contain: 'strict',
                    textAlign: 'left',
                    display: 'inline-block',
                    overflow: 'hidden',
                    fontSize: '1.1em',
                  }}
                />

                <b>Z: </b>
                <Printer
                  accessor="zd"
                  precision={3}
                  style={{
                    width: '4em',
                    height: '1.05em',
                    contain: 'strict',
                    textAlign: 'left',
                    display: 'inline-block',
                    overflow: 'hidden',
                    fontSize: '1.1em',
                  }}
                />
              </Card>
            </Areas.Numeric>

            <Areas.TimeSlice>
              <Card>
                <h1>Time Slice</h1>
                <div style={{ textAlign: 'center', marginBottom: '1em' }}>
                  <b>XY Acceleration</b>
                </div>
                <ChartContainer height="43vh">
                  <TimeSlicedLineChart
                    dataSource={accDS}
                    accessor={(data, time) => ({ x: data[0], y: data[2] })}
                    color={'#eaff00'}
                    lineWidth={4}
                  />
                  <RealTimeSlicingDomain
                    window={200}
                    xMin={-20}
                    xMax={20}
                    yMin={-20}
                    yMax={20}
                  />
                  <Fog color="#a1978c" />

                  <HorizontalAxis label="X m/s²" />
                  <VerticalAxis label="Y m/s²" />
                </ChartContainer>
              </Card>
            </Areas.TimeSlice>
            <Areas.Model>
              <Card>
                <Environment
                  camera={{
                    fov: 50,
                    position: [0, 0, -85],
                  }}
                  style={{ width: '100%', height: '25vh' }}
                >
                  {/* <OrbitControls /> */}
                  <ControlledGroup
                    position={[0, 0, 0]}
                    // positionAccessor={state => {
                    //   return [state.acc[1] / 10, state.acc[2] / 10, state.acc[0] / 10]
                    // }}
                    rotationAccessor={state => {
                      return [
                        state.y,
                        state.z,
                        state.x,
                      ]
                    }}
                  >
                    <GLTF asset={IMUModel} />
                  </ControlledGroup>
                  <ambientLight intensity={0.1} />
                  <hemisphereLight intensity={0.3} />
                  <directionalLight intensity={1.0} />
                </Environment>
              </Card>
            </Areas.Model>
          </React.Fragment>
        )}
      </Composition>
    </React.Fragment>
  )
}
