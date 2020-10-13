let debug = (msg, obj) => {
  Js.log("# " ++ msg);
  Js.log(obj);
};

let decodeFail = msg => Json.Decode.DecodeError(msg) |> raise;
