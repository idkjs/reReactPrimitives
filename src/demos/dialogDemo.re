module MyLayerManager = LayerManager.Make LayerManager.DefaultImpl;

let openDialog _ => {
  let layer = MyLayerManager.make FullViewport;
  MyLayerManager.render
    layer
    <Dialog
      title="Hello"
      description="This is a dialog"
      onPressClose=(fun _ => MyLayerManager.remove layer)
      minWidth=300>
      <div style=(ReactDOMRe.Style.make padding::"10px" ())>
        (ReasonReact.stringToElement "This is a dialog")
      </div>
    </Dialog>
};

ReactDOMRe.renderToElementWithId
  <div> <Button title="Open dialog" color="#fb5" onPress=openDialog /> </div> "root";
