import {
  ChartContainer,
  LineChart,
  RealTimeDomain,
  TimeAxis,
  VerticalAxis,
  TriggerDomain,
} from '@electricui/components-desktop-charts'

import { Card, Colors } from '@blueprintjs/core'
import { Composition } from 'atomic-layout'
import { RouteComponentProps } from '@reach/router'
import { MessageDataSource } from '@electricui/core-timeseries'
import { Printer } from '@electricui/components-desktop'
import React, { useEffect, useRef, useState } from 'react'
import { Slider } from '@electricui/components-desktop-blueprint'
import { Statistic, Statistics } from '@electricui/components-desktop-blueprint'
import { ProgressBar } from '@electricui/components-desktop-blueprint'
import { HTMLTable } from '@blueprintjs/core'
import { DataSourcePrinter } from '@electricui/components-desktop-charts'
import { HelpPopover } from 'src/application/components/HelpPopover/HelpPopover'
import { Callout } from '@blueprintjs/core'

const layoutDescription = `
    Controls Map 
    Depth Pressure
    Salinity Temperature
  `

const averageSpeedDS = new MessageDataSource('as')
const hitlDS = new MessageDataSource('hd')

export const HITLPage = (props: RouteComponentProps) => {
  return (
    <React.Fragment>
      <Composition areas={layoutDescription} gap={10} autoCols="1fr">
        {Areas => (
          <React.Fragment>
            <Areas.Controls>
              <Card>
                <h1>Hardware in the Loop</h1>

                <Statistics>
                  <Statistic
                    label="Data Index"
                    accessor="ind"
                    color="#9f7ef7"
                  />
                  <Statistic
                    label="Time (hours)"
                    accessor="ts"
                    color={Colors.BLUE5}
                  />
                  <Statistic
                    label="HITL Duty (ms)"
                    accessor="hr"
                    color={Colors.RED5}
                  />
                  <Statistic
                    label="Progress"
                    accessor="hp"
                    suffix="%"
                    color={Colors.GREEN5}
                  />
                </Statistics>
                <br></br>
                <h3>Progress</h3>
                <ProgressBar
                  accessor="hp"
                  min={0}
                  max={100}
                  intent="success"
                  stripes
                  animate={true}
                />

                <h3>HITL Speed</h3>
                <Slider
                  min={0}
                  max={2}
                  stepSize={0.01}
                  sendOnlyOnRelease={false}
                  labelStepSize={0.25}
                  labelRenderer={(val: number) => `${Math.round(val * 100)}%`}
                  defaultTrackIntent="primary"
                >
                  <Slider.Handle accessor="hds" />
                </Slider>

                <br></br>
                <br></br>

                <h2>HITL Data Information</h2>
                <HTMLTable striped interactive>
                  <thead></thead>
                  <tbody>
                    <tr>
                      <td>Dataset</td>
                      <td>silbo-20180515T1016</td>
                    </tr>
                    <tr>
                      <td>Deployment Date</td>
                      <td>May 25, 2018</td>
                    </tr>
                    <tr>
                      <td>Recovery Date</td>
                      <td>July 16, 2019</td>
                    </tr>
                    <tr>
                      <td>Source</td>
                      <td>Teledyne Webb Research</td>
                    </tr>
                    <tr>
                      <td>Glider Type</td>
                      <td>Slocum</td>
                    </tr>
                  </tbody>
                </HTMLTable>
              </Card>
            </Areas.Controls>
            <Areas.Map>
              <Card>
                <h1>Navigation</h1>
                <Statistic
                  label="Latitude"
                  accessor="lat"
                  suffix="°"
                  precision={4}
                  color="#9f7ef7"
                />

                <Statistic
                  label="Longitude"
                  accessor="lon"
                  suffix="°"
                  precision={4}
                  color={Colors.RED5}
                />
                <br></br>
                <br></br>
                <Statistic
                  label="Average Speed"
                  accessor="as"
                  suffix="km/h"
                  precision={4}
                  color={Colors.CERULEAN5}
                />

                <Statistic
                  label="Distance"
                  accessor="td"
                  suffix="km"
                  precision={4}
                  color={Colors.GREEN5}
                />
                <br></br>
                <br></br>

                <Callout title="Data Quality Control" intent="warning">
                  Frequent changes in the duty cycle will result in inaccurate speed calculations.
                </Callout>

                <br></br>
                <br></br>

                <ChartContainer>
                  <LineChart
                    dataSource={averageSpeedDS}

                    color={Colors.BLUE5}
                  />
                  <RealTimeDomain window={30000} />
                  <TimeAxis label="Time (s)" />
                  <VerticalAxis label="Speed (km/h)" />
                </ChartContainer>

              </Card>
            </Areas.Map>
            <Areas.Depth>
              <Card>
                <div style={{ textAlign: 'center', marginBottom: '1em' }}>
                  <b>Depth</b>
                </div>
                <DataSourcePrinter
                  dataSource={hitlDS}
                  accessor={(data, time) => data[0]}
                />{' '}
                meters
                <br></br>
                <br></br>
                <ChartContainer>
                  <LineChart
                    dataSource={hitlDS}
                    accessor={(data, time) => data[0]}
                    color={Colors.RED5}
                  />
                  <RealTimeDomain window={10000} />
                  <TimeAxis label="Time (s)" />
                  <VerticalAxis label="Depth (m)" />
                </ChartContainer>
              </Card>
            </Areas.Depth>
            <Areas.Pressure>
              <Card>
                <div style={{ textAlign: 'center', marginBottom: '1em' }}>
                  <b>Pressure</b>
                </div>
                <DataSourcePrinter
                  dataSource={hitlDS}
                  accessor={(data, time) => data[1]}
                />{' '}
                atm
                <br></br>
                <br></br>
                <ChartContainer>
                  <LineChart
                    dataSource={hitlDS}
                    accessor={(data, time) => data[1]}
                    color={Colors.GREEN4}
                  />
                  <RealTimeDomain window={10000} />
                  <TimeAxis label="Time (s)" />
                  <VerticalAxis label="Pressure (atm)" />
                </ChartContainer>
              </Card>
            </Areas.Pressure>
            <Areas.Salinity>
              <Card>
                <div style={{ textAlign: 'center', marginBottom: '1em' }}>
                  <b>Salinity</b>
                </div>
                <DataSourcePrinter
                  dataSource={hitlDS}
                  accessor={(data, time) => data[1]}
                />{' '}
                g/L
                <br></br>
                <br></br>
                <ChartContainer>
                  <LineChart
                    dataSource={hitlDS}
                    accessor={(data, time) => data[2]}
                    color={Colors.BLUE5}
                  />
                  <RealTimeDomain window={10000} />
                  <TimeAxis label="Time (s)" />
                  <VerticalAxis label="Salinity (g/L)" />
                </ChartContainer>
              </Card>
            </Areas.Salinity>
            <Areas.Temperature>
              <Card>
                <div style={{ textAlign: 'center', marginBottom: '1em' }}>
                  <b>Temperature</b>
                </div>
                <DataSourcePrinter
                  dataSource={hitlDS}
                  accessor={(data, time) => data[3]}
                />{' '}
                Celsius
                <br></br>
                <br></br>
                <ChartContainer>
                  <LineChart
                    dataSource={hitlDS}
                    accessor={(data, time) => data[3]}
                    color={Colors.ORANGE5}
                  />
                  <RealTimeDomain window={10000} />
                  <TimeAxis label="Time (s)" />
                  <VerticalAxis label="Temperature (C)" />
                </ChartContainer>
              </Card>
            </Areas.Temperature>
          </React.Fragment>
        )}
      </Composition>
    </React.Fragment>
  )
}
