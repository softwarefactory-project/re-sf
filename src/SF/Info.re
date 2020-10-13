// A software factory info context
type info = {
  services: list(service),
  version: string,
}
and service = {
  name: string,
  path: string,
};

let parseService = (json): service =>
  Json.Decode.{
    name: json |> field("name", string),
    path: json |> field("path", string),
  };

let parse = (json): info =>
  Json.Decode.{
    version: json |> field("version", string),
    services: json |> field("services", list(parseService)),
  };
