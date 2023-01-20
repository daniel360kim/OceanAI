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
  ZoomBrush,
} from '@electricui/components-desktop-charts'

import {
  Card,
  Colors,
} from '@blueprintjs/core'
import { Composition } from 'atomic-layout'
import { IntervalRequester } from '@electricui/components-core'
import { DataSource, MessageDataSource } from '@electricui/core-timeseries'
import React from 'react'
import { RouteComponentProps } from '@reach/router'

const rxDS = new MessageDataSource('xd')
const ryDS = new MessageDataSource('yd')
const rzDS = new MessageDataSource('zd')

const accDS = new MessageDataSource('ad')
const gyrDS = new MessageDataSource('gd')

const extPresDS = new MessageDataSource('ep')

const navigationLayoutDescription = `
    Chart TimeSlice
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
                  <RealTimeDomain window={30000} />
                  <TimeAxis label="Time (s)" />
                  <VerticalAxis label="Orientation (deg)" />
                  <ZoomBrush />
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

                  <RealTimeDomain window={30000} />
                  <TimeAxis label="Time (s)" />
                  <VerticalAxis label="Angular Rate (rad/s)" />
                  <ZoomBrush />
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
                  <RealTimeDomain window={30000} />
                  <TimeAxis label="Time (s)" />
                  <VerticalAxis label="Acceleration (m/s/s)" />
                  <ZoomBrush />
                </ChartContainer>

                <ChartContainer>
                <LineChart
                    dataSource={extPresDS}
                    color={Colors.RED5}
                  />

                  <RealTimeDomain window={30000} />
                  <TimeAxis label="Time (s)" />
                  <VerticalAxis label="Acceleration (m/s/s)" />
                  <ZoomBrush />
                </ChartContainer>
                
                
              </Card>
            </Areas.Chart>

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

                <ChartContainer height="43vh">
                  <TimeSlicedLineChart
                    dataSource={gyrDS}
                    accessor={(data, time) => ({ x: data[0], y: data[2] })}
                    color={'#eb34c3'}
                    lineWidth={4}
                  />
                  <RealTimeSlicingDomain
                    window={200}
                    xMin={-7}
                    xMax={7}
                    yMin={-7}
                    yMax={7}
                  />
                  <Fog color="#a1978c" />
                  <HorizontalAxis label="X rad/s" />
                  <VerticalAxis label="Y rad/s" />
                </ChartContainer>
              </Card>
            </Areas.TimeSlice>
          </React.Fragment>
        )}
      </Composition>
    </React.Fragment>
  )
}
