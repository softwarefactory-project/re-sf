[@bs.val] external __dirname: string = "__dirname";
let getAbsolutePath = (path: string) => {
  Node.Path.resolve(__dirname, path);
};

let readFile = (path: string) => {
  let absolutePath = getAbsolutePath(path);
  Node.Fs.readFileSync(absolutePath, `utf8);
};

let debug = (msg, obj) => {
  Js.log("# " ++ msg);
  Js.log(obj);
};

let decodeFail = msg => Json.Decode.DecodeError(msg) |> raise;
