type state = {
  height: option(int),
  inputRef: ref(option(Dom.element)),
  focused: bool,
};

type action =
  | Change
  | Focus
  | Blur
  | SetHeight(int);

let component = ReasonReact.reducerComponent("TextInput");

[@bs.get]
external getStyle : DomRe.Element.t => Dom.cssStyleDeclaration = "style";

let setInputRef = (inputRef, {ReasonReact.state}) =>
  state.inputRef := Js.Nullable.toOption(inputRef);

/* TODO: manage types and local validation */
let make =
    (
      ~multiline=false,
      ~autoSize=false,
      ~autoFocus=false,
      ~disabled=false,
      ~type_="text",
      ~id=?,
      ~name=?,
      ~value,
      ~maxLength=?,
      ~style=?,
      ~onTextChange,
      ~onKeyDown=?,
      ~onPaste=?,
      ~onFocus=?,
      ~onBlur=?,
      ~focusedStyle=?,
      ~placeholder="",
      ~rows=1,
      _children,
    ) => {
  let measureAndSetHeight = ((), {ReasonReact.state, ReasonReact.send}) =>
    switch (state.inputRef) {
    | {contents: Some(element)} =>
      CssStyleDeclarationRe.setProperty(
        "height",
        "0",
        "",
        getStyle(element),
      );
      let height = DomRe.Element.scrollHeight(element);
      CssStyleDeclarationRe.setProperty(
        "height",
        string_of_int(height) ++ "px",
        "",
        getStyle(element),
      );
      send(SetHeight(height));
    | _ => ()
    };
  let handleResize = ({ReasonReact.handle}) =>
    if (multiline && autoSize) {
      Webapi.requestAnimationFrame((_) => handle(measureAndSetHeight, ()));
    };
  let handleChange = self => handleResize(self);
  {
    ...component,
    initialState: () => {height: None, inputRef: ref(None), focused: false},
    didMount: self => handleResize(self),
    reducer: (action, state) =>
      switch (action) {
      | Focus => ReasonReact.Update({...state, focused: true})
      | Blur => ReasonReact.Update({...state, focused: false})
      | Change => ReasonReact.SideEffects(handleChange)
      | SetHeight(height) =>
        ReasonReact.Update({...state, height: Some(height)})
      },
    render: ({send, state, handle}) => {
      let sizingStyle =
        ReactDOMRe.Style.make(
          ~resize="none",
          ~boxSizing="content-box",
          ~fontSize="16px",
          ~height=
            switch (state.height) {
            | None => "auto"
            | Some(height) => string_of_int(height) ++ "px"
            },
          (),
        );
      ReactDOMRe.createElement(
        multiline ? "textarea" : "input",
        ~props=
          ReactDOMRe.props(
            ~ref=handle(setInputRef),
            ~rows,
            ~type_,
            ~id?,
            ~name?,
            ~disabled=disabled,
            ~style=
              ReactDOMRe.Style.combine(
                switch (style) {
                | Some(style) => ReactDOMRe.Style.combine(style, sizingStyle)
                | None => sizingStyle
                },
                switch (focusedStyle) {
                | Some(style) when state.focused == true => style
                | _ => ReactDOMRe.Style.make()
                },
              ),
            ~onChange=
              event => {
                onTextChange(
                  ReactDOMRe.domElementToObj(ReactEventRe.Form.target(event))##value,
                );
                send(Change);
              },
            ~onKeyDown?,
            ~onPaste?,
            ~onFocus=
              event => {
                switch (onFocus) {
                | Some(onFocus) => onFocus(event)
                | None => ()
                };
                send(Focus);
              },
            ~onBlur=
              event => {
                switch (onBlur) {
                | Some(onBlur) => onBlur(event)
                | None => ()
                };
                send(Blur);
              },
            ~value,
            ~maxLength?,
            ~placeholder,
            ~autoFocus=autoFocus,
            (),
          ),
        [||],
      );
    },
  };
};
