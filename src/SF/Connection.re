let example = {|
{
  "base-url": "https://pagure.io",
  "type": "pagure",
  "name": "pagure.io"
}
|};

type connection = {
  name: string,
  connection_type: string,
  base_url: option(string),
};

let parseConnection = json => {
  Json.Decode.{
    connection_type: json |> field("type", string),
    name: json |> field("name", string),
    base_url: json |> optional(field("base-url", string)),
  };
};

let runExample = () => {
  Js.log("Running connection example");
  let json = Json.parseOrRaise(example);
  let connections = parseConnection(json);
  Js.log(connections)
};
