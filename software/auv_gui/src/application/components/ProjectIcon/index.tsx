import React from 'react'

import logo from './earth.png'
import CSS from 'csstype'

type ProjectIconProps = {
    style?: React.CSSProperties
    containerStyle?: React.CSSProperties
    width?: CSS.Properties['width']
    height?: CSS.Properties['height']
}

export const ProjectIcon = (props: ProjectIconProps) => {
    return (
        <img
            src={logo}
            style={props.style}
            width={props.width}
            height={props.height}
        />
    )
}