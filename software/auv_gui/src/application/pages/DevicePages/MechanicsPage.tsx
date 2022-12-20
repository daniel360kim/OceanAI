import React from 'react'
import { RouteComponentProps } from '@reach/router'
import { ButtonGroup, Card, Colors } from '@blueprintjs/core'
import { ProgressBar } from '@electricui/components-desktop-blueprint'
import { MessageDataSource } from '@electricui/core-timeseries'
import { Composition } from 'atomic-layout'
import { StateIndicator } from 'src/application/components/StateIndication'
import { Statistic, Statistics } from '@electricui/components-desktop-blueprint'
import { NumberInput } from '@electricui/components-desktop-blueprint'
import { Popover } from '@blueprintjs/core'
import { Button as BlueprintButton } from '@blueprintjs/core'
import { Button } from '@electricui/components-desktop-blueprint'

import {
  ChartContainer,
  LineChart,
  RealTimeDomain,
  TimeAxis,
  VerticalAxis,
  ZoomBrush,
} from '@electricui/components-desktop-charts'

const currentPositionDS = new MessageDataSource('sp')
const targetPositionDS = new MessageDataSource('st')
const stepSpeedDS = new MessageDataSource('ss')
const stepAccelDS = new MessageDataSource('sa')

const navigationLayoutDescription = `
    Controls Charts
`

export const MechanicsPage = (props: RouteComponentProps) => {
  return (
    <React.Fragment>
      <Composition areas={navigationLayoutDescription} gap={10} autoCols="1fr">
        {Areas => (
          <React.Fragment>
            <Areas.Controls>
              <Card>
                <ProgressBar
                  accessor="sp"
                  min={0}
                  max={27000}
                  intent="success"
                  stripes
                />

                <br></br>

                <Statistics>
                  <Statistic
                    label="Current Position (half steps)"
                    accessor="sp"
                    color={Colors.BLUE5}
                  />
                  <Statistic
                    label="Target Position (half steps)"
                    accessor="st"
                    color={Colors.RED5}
                  />
                </Statistics>
                <br></br>
                <Statistics>
                  <Statistic
                    label="Current Speed"
                    accessor="ss"
                    color={Colors.GREEN5}
                  />
                  <Statistic
                    label="Current Acceleration"
                    accessor="sa"
                    color={Colors.ORANGE5}
                  />
                </Statistics>
              </Card>
              <StateIndicator />
              <Card>
                <Popover>
                  <BlueprintButton large intent="warning" icon="dashboard">
                    Stepper Settings
                  </BlueprintButton>

                  <div style={{ padding: '20px' }}>
                    <h3>Stepper Settings</h3>
                    <h4>Acceleration</h4>
                    <NumberInput
                      accessor="ac"
                      intent="warning"
                      min={0}
                      max={2000}
                      leftIcon="dashboard"
                      large
                    />
                    <h4>Speed</h4>
                    <NumberInput
                      accessor="sc"
                      intent="success"
                      min={0}
                      max={2000}
                      leftIcon="dashboard"
                      large
                    />
                    <br />
                    <ButtonGroup>
                      <Button
                        large
                        intent="danger"
                        writer={state => {
                          state.ssc = 1
                        }}
                      >
                        Pause
                      </Button>
                      <Button
                        large
                        intent="success"
                        writer={state => {
                          state.ssc = 0
                        }}
                      >
                        Resume
                      </Button>
                    </ButtonGroup>
                  </div>
                </Popover>

                <br></br>
              </Card>
            </Areas.Controls>
            <Areas.Charts>
              <Card>
                <div style={{ textAlign: 'center', marginBottom: '1em' }}>
                  <b>Stepper Position</b>
                </div>
                <ChartContainer>
                  <LineChart dataSource={currentPositionDS} />
                  <RealTimeDomain window={30000} />
                  <TimeAxis label="Time (s)" />
                  <VerticalAxis />
                  <ZoomBrush />
                </ChartContainer>
              </Card>
      
            </Areas.Charts>
          </React.Fragment>
        )}
      </Composition>
    </React.Fragment>
  )
}
