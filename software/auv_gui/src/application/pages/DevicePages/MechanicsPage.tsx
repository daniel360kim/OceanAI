import React from 'react'
import { RouteComponentProps } from '@reach/router'
import { ButtonGroup, Card, Colors } from '@blueprintjs/core'
import { ProgressBar, Switch } from '@electricui/components-desktop-blueprint'
import { MessageDataSource } from '@electricui/core-timeseries'
import { Composition } from 'atomic-layout'
import { StateIndicator } from 'src/application/components/StateIndication'
import { Statistic, Statistics } from '@electricui/components-desktop-blueprint'
import { NumberInput } from '@electricui/components-desktop-blueprint'
import { Popover } from '@blueprintjs/core'
import { Button as BlueprintButton } from '@blueprintjs/core'
import { Button } from '@electricui/components-desktop-blueprint'
import { Checkbox } from '@electricui/components-desktop-blueprint'
import { HueSlider } from '@electricui/components-desktop'
import { Slider } from '@electricui/components-desktop-blueprint'



const navigationLayoutDescription = `
    BControls 
    PControls
`

export const MechanicsPage = (props: RouteComponentProps) => {
  return (
    <React.Fragment>
      <Composition areas={navigationLayoutDescription} gap={10} autoCols="1fr">
        {Areas => (
          <React.Fragment>
            <Areas.BControls>
              <Card>
                <h1>Ballast Control</h1>
                <ProgressBar
                  accessor="bsp"
                  min={0}
                  max={27000}
                  intent="success"
                  stripes
                />

                <br></br>

                <Statistics>
                  <Statistic
                    label="Current Position (half steps)"
                    accessor="bsp"
                    color={Colors.BLUE5}
                  />
                  <Statistic
                    label="Target Position (half steps)"
                    accessor="bst"
                    color={Colors.RED5}
                  />
                </Statistics>
                <br></br>
                <Statistics>
                  <Statistic
                    label="Current Speed"
                    accessor="bss"
                    color={Colors.GREEN5}
                  />
                  <Statistic
                    label="Current Acceleration"
                    accessor="bsa"
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
                      accessor="bac"
                      intent="warning"
                      min={0}
                      max={2000}
                      leftIcon="dashboard"
                      large
                    />
                    <h4>Speed</h4>
                    <NumberInput
                      accessor="bsc"
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
                        icon="pause"
                        writer={state => {
                          state.ssc = 1
                        }}
                      >
                        Pause
                      </Button>
                      <Button
                        large
                        intent="success"
                        icon="play"
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
            </Areas.BControls>

            <Areas.PControls>
              <Card>
                <h1>Pitch Control</h1>
                <Checkbox
                  accessor="ap"
                  checked={1}
                  unchecked={0}
                  writer={(state, value) => {
                    state.ap = value
                  }}
                >
                  Manual Control
                </Checkbox>
                <br></br>
                <ProgressBar
                  accessor="psp"
                  min={0}
                  max={10850}
                  intent="success"
                  stripes
                />

                <br></br>

                <Statistics>
                  <Statistic
                    label="Current Position (half steps)"
                    accessor="psp"
                    color={Colors.BLUE5}
                  />
                  <Statistic
                    label="Target Position (half steps)"
                    accessor="pst"
                    color={Colors.RED5}
                  />
                </Statistics>
                <br></br>
                <Statistics>
                  <Statistic
                    label="Current Speed"
                    accessor="pss"
                    color={Colors.GREEN5}
                  />
                  <Statistic
                    label="Current Acceleration"
                    accessor="psa"
                    color={Colors.ORANGE5}
                  />
                </Statistics>
              </Card>
              <Card>
                <Popover>
                  <BlueprintButton large intent="warning" icon="dashboard">
                    Stepper Settings
                  </BlueprintButton>

                  <div style={{ padding: '20px' }}>
                    <h3>Stepper Settings</h3>
                    <h4>Acceleration</h4>
                    <NumberInput
                      accessor="pac"
                      intent="warning"
                      min={0}
                      max={2000}
                      leftIcon="dashboard"
                      large
                    />
                    <h4>Absolute Speed</h4>
                    <NumberInput
                      accessor="psc"
                      intent="success"
                      min={0}
                      max={2000}
                      leftIcon="dashboard"
                      large
                    />
                    <br />

                    <ButtonGroup>
                      <Button
                        writer={state => {
                          state.pd = 0
                        }}
                        intent="primary"
                        large
                        icon="arrow-left"
                      >
                        Forward
                      </Button>
                      <Button
                        writer={state => {
                          state.pd = 1
                        }}
                        intent="primary"
                        large
                        icon="arrow-right"
                      >
                        Backward
                      </Button>
                    </ButtonGroup>
                    <br />
                    <br />
                    <ButtonGroup>
                      <Button
                        writer={state => {
                          state.pr = 0
                        }}
                        intent="success"
                        large
                        icon="play"
                      >
                        Resume
                      </Button>
                      <Button
                        writer={state => {
                          state.pr = 1
                        }}
                        intent="danger"
                        large
                        icon="pause"
                      >
                        Reset
                      </Button>
                    </ButtonGroup>
                    <br></br>
                    <br></br>
                    <Button
                      writer={state => {
                        state.psc = 0
                      }}
                      intent="danger"
                      large
                      icon="stop"
                      fill
                    >
                      Stop
                    </Button>
                  </div>
                </Popover>

                <br></br>
              </Card>
            </Areas.PControls>

          </React.Fragment>
        )}
      </Composition>
    </React.Fragment>
  )
}
