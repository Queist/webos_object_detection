
import LS2Request from '@enact/webos/LS2Request';
import 'bootstrap/dist/css/bootstrap.css';
import Button from 'react-bootstrap/Button';
import Form from 'react-bootstrap/Form';
import InputGroup from 'react-bootstrap/InputGroup';
import React, {useState} from "react";
import {DropdownButton, ToggleButton, Dropdown, Stack} from "react-bootstrap";
import 'bootstrap/dist/css/bootstrap.min.css';
import './App.css'

let webOSBridge = new LS2Request();
let glEffectsTitle = [
    "No GlEffect",
    "Mirror",
    "Squeeze",
    "Stretch",
    "Tunnel",
    "FishEye",
    "Twirl",
    "Bulge",
    "Square",
    "Heat",
    "Sepia",
    "Xpro",
    "Lumaxpro",
    "Xray",
    "Sin",
    "Glow",
    "Sobel",
    "Blur",
    "Laplacian"
]
let glEffectsButton = [
    "None",
    "Mirror",
    "Squeeze",
    "Stretch",
    "Tunnel",
    "FishEye",
    "Twirl",
    "Bulge",
    "Square",
    "Heat",
    "Sepia",
    "Xpro",
    "Lumaxpro",
    "Xray",
    "Sin",
    "Glow",
    "Sobel",
    "Blur",
    "Laplacian"
]

function ObjectDetectButton(props) {
  return (
      <>
        <InputGroup className="mb-3" style={{display: "flex", padding: "10px"}}>
          <Button variant="outline-secondary" size="sm" id="button-addon1" onClick={props.onClick}>
            Use Local File
          </Button>
          <Form.Control
              onChange={props.onChange}
              placeholder="Input URI"
              aria-label="Example text with button addon"
              aria-describedby="basic-addon1"
              value={props.uri}
          />
        </InputGroup>
      </>
  )
}

function App() {
    let [uri, setUri] = useState("")
    let [useObjectDetection, setUseObjectDetection] = useState(false)
    let [glEffect, setGlEffect] = useState(0)

    return (
        <div className="App">
            <header className="App-header">
                <div style={{display: "flex", padding: "10px"}}>
                    <ToggleButton size='sm' id="tbg-btn-1" type="checkbox" value={1} checked={useObjectDetection} onChange={() => setUseObjectDetection(!useObjectDetection)} >
                        Use Object Detection
                    </ToggleButton>
                </div>
                <DropdownButton
                    style={{padding: "10px"}}
                    className="btn-outline-secondary"
                    variant="outline-secondary"
                    title={glEffectsTitle[glEffect]}
                    id="dropdown-basic-button"
                >
                    <div style={{display: "flex", flexDirection: "column"}}>
                        {glEffectsButton.map((value, index) => {
                            return (
                                <Dropdown.Item href="#"
                                               active={glEffect === index}
                                               onClick={() => setGlEffect(index)}>
                                    {value}
                                </Dropdown.Item>
                            )
                        })}
                    </div>
                </DropdownButton>
                <ObjectDetectButton onClick={() => {
                    const lsRequest = {
                        "service": "luna://com.balance.app.service",
                        "method": "file",
                        "parameters": {
                            "url": uri,
                            "objectDetection": useObjectDetection,
                            "glEffect": glEffect
                        },
                        "onSuccess": (ignored) => {
                        },
                        "onFailure": (ignored) => {
                        }
                    };
                    webOSBridge.send(lsRequest)
                    setUri("")
                }} onChange={(e) => {
                    setUri(e.target.value)
                }} uri={uri}
                />
            </header>
        </div>
    );
}

export default App;
